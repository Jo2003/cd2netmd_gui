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
#include <cdio/cdio.h>
#include "defines.h"

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param[in]  tracks  The audio tracks vector
//! @param      parent  The parent
//--------------------------------------------------------------------------
CCDItemModel::CCDItemModel(const c2n::AudioTracks &tracks,
                           QObject *parent)
    :QAbstractTableModel(parent)
{
    for(const auto& t : tracks)
    {
        mTitles << t.mTitle;
        mTTimes << (t.mLbCount / CDIO_CD_FRAMES_PER_SEC);
    }
}

CCDItemModel::~CCDItemModel()
{
    // cleanup time
}

int CCDItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mTitles.size();
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
            return mTitles.at(row);
        case 1:
            {
                time_t secs = mTTimes.at(row);
                return QString("%1:%2:%3")
                    .arg((int)(secs / 3600), 1, 10, QChar('0'))
                    .arg((int)((secs % 3600) / 60), 2, 10, QChar('0'))
                    .arg((int)(secs % 60), 2, 10, QChar('0'));
            }
        }
    }
    else if ((role == Qt::EditRole) && (col == 0))
    {
        return mTitles.at(row);
    }
    else if ((role == Qt::DecorationRole) && (col == 0))
    {
        return QIcon(":/view/audio");
    }
    else if ((role == Qt::UserRole) && (col == 1))
    {
        // return track time in seconds
        return static_cast<int>(mTTimes.at(row));
    }

    return QVariant();
}

bool CCDItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        if (col == 0)
        {
            mTitles[row] = value.toString();
            return true;
        }
    }
    return QAbstractTableModel::setData(index, value, role);
}

Qt::ItemFlags CCDItemModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
    {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    return QAbstractTableModel::flags(index);
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
