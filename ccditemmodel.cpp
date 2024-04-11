/**
 * Copyright (C) 2021 Jo2003 (olenka.joerg@gmail.com)
 * This file is part of cd2netmd_gui
 *
 * cd2netmd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cd2netmd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#include "ccditemmodel.h"
#include <QFont>
#include <QIcon>
#include <QMimeData>
#include <cdio/cdio.h>
#include "ccditemmodel.h"
#include "defines.h"

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param[in]  tracks  The audio tracks vector
//! @param      parent  The parent
//--------------------------------------------------------------------------
CCDItemModel::CCDItemModel(const c2n::AudioTracks &tracks,
                           QObject *parent)
    :QAbstractTableModel(parent), mTracks(tracks)
{
}

CCDItemModel::~CCDItemModel()
{
    // cleanup time
}

int CCDItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mTracks.size();
}

int CCDItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant CCDItemModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole)
    {
        switch (col)
        {
        case 0:
            return mTracks.at(row).mTitle;
        case 1:
            {
                float secs = static_cast<float>(mTracks.at(row).mLbCount) / static_cast<float>(CDIO_CD_FRAMES_PER_SEC);
                int h = static_cast<int>(secs) / 3600;
                int m = (static_cast<int>(secs) % 3600) / 60;

                float fsec = secs - static_cast<float>(h * 3600 + m * 60);
                return QString("%1:%2:%3")
                    .arg(h, 1, 10, QChar('0'))
                    .arg(m, 2, 10, QChar('0'))
                    .arg(fsec, 5, 'f', 2, QChar('0'));
            }
        default:
            return QVariant();
        }
    }
    else if ((role == Qt::EditRole) && (col == 0))
    {
        return mTracks.at(row).mTitle;
    }
    else if ((role == Qt::DecorationRole) && (col == 0))
    {
        return QIcon(":/view/audio");
    }
    else if ((role == Qt::UserRole) && (col == 1))
    {
        // return track time in seconds
        return static_cast<double>(mTracks.at(row).mLbCount) / static_cast<double>(CDIO_CD_FRAMES_PER_SEC);
    }
    else if ((role == TSTAMP_ROLE) && (col == 1))
    {
        return mTracks.at(row).mTStamp;
    }

    return QVariant();
}

bool CCDItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    if (role == Qt::EditRole)
    {
        if (col == 0)
        {
            mTracks[row].mTitle = value.toString();
            return true;
        }
    }

    return QAbstractTableModel::setData(index, value, role);
}

QVariant CCDItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return QString("Title");
            case 1:
                return QString("Time");
            }
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

//--------------------------------------------------------------------------
//! @brief      get item flags
//!
//! @param[in]  index  The index
//!
//! @return     item flags
//--------------------------------------------------------------------------
Qt::ItemFlags CCDItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid())
    {
        return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    else
    {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

//--------------------------------------------------------------------------
//! @brief      tells what dropactions are supported
//!
//! @return     supported drop actions
//--------------------------------------------------------------------------
Qt::DropActions CCDItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//--------------------------------------------------------------------------
//! @brief      export mime types
//--------------------------------------------------------------------------
QStringList CCDItemModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

//--------------------------------------------------------------------------
//! @brief      remove [count] rows starting from [row]
//!
//! @param[in]  row          target row
//! @param[in]  count        row count
//! @param[in]  parent       parent modell index
//!
//! @return     true if handled
//--------------------------------------------------------------------------
bool CCDItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if ((row > -1) && (row > -1))
    {
        beginRemoveRows(parent, row, row + count - 1);
        for(int i = (count - 1); i >= 0; i--)
        {
            if ((i + row) < mTracks.size())
            {
                mTracks.remove(i + row);
            }
        }
        endRemoveRows();
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------
//! @brief      get length of audio in kist
//!
//! @return     length in blocks
//--------------------------------------------------------------------------
long CCDItemModel::audioLength() const
{
    long length = 0;
    for (const auto& t : mTracks)
    {
        length += t.mLbCount;
    }
    return length;
}

//--------------------------------------------------------------------------
//! @brief      return current audio tracks
//!
//! @return     audio tracks
//--------------------------------------------------------------------------
c2n::AudioTracks CCDItemModel::audioTracks() const
{
    return mTracks;
}

//--------------------------------------------------------------------------
//! @brief      Information carried when dragging
//!
//! @return     pointer to mime data
//--------------------------------------------------------------------------
QMimeData *CCDItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for(const auto& index : indexes)
    {
        if (index.isValid() && (index.column() == 0))
        {
            if (index.column() == 0)
            {
                // insert source row
                stream << QString::number(index.row());
            }
        }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

//--------------------------------------------------------------------------
//! @brief      Process the drop according to the information carried by drag
//!
//! @param[in]  data         mime date needed for drop
//! @param[in]  action       what needs to be done
//! @param[in]  row          target row
//! @param[in]  column       target column
//! @param[in]  parent       parent modell index
//!
//! @return     true if handled
//--------------------------------------------------------------------------
bool CCDItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    if (!data->hasFormat("application/vnd.text.list"))
    {
        return false;
    }

    if (column > 0)
    {
        return false;
    }

    int beginRow;

    if (row != -1)
    {
        beginRow = row;
    }
    else if (parent.isValid())
    {
        beginRow = parent.row();
    }
    else
    {
        beginRow = rowCount(QModelIndex());
    }

    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QVector<int> srcRows;

    while (!stream.atEnd())
    {
        QString text;
        stream >> text;
        srcRows << text.toInt();
    }

    std::sort(srcRows.begin(), srcRows.end());

    qInfo() << "Drop rows" << srcRows << "into row" << beginRow;

    // create temp audio tracks vector for sorting
    c2n::AudioTracks tmpTracks;
    bool tracksHandled = false;

    for (int i = 0; i < mTracks.size(); i++)
    {
        if (i == beginRow)
        {
            tracksHandled = true;
            for (auto t : srcRows)
            {
                tmpTracks.append(mTracks.at(t));
            }
        }

        if (!srcRows.contains(i))
        {
            tmpTracks.append(mTracks.at(i));
        }
    }

    if (!tracksHandled)
    {
        // append
        for (auto t : srcRows)
        {
            tmpTracks.append(mTracks.at(t));
        }
    }

    mTracks = tmpTracks;
    return true;
}
