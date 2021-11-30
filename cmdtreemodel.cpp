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
#include "cmdtreemodel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QIcon>

CMDTreeModel::CMDTreeModel(const QString& jsonContent, QObject *parent)
    :QAbstractItemModel(parent)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8());

    if (doc.isObject())
    {
        mMDJson = doc.object();
    }

    // root item with column names
    mpTreeRoot = new CTreeItem({tr("Name"), tr("Mode"), tr("Time")});
    setupModelData();
}

CMDTreeModel::~CMDTreeModel()
{
    delete mpTreeRoot;
}

QModelIndex CMDTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = mpTreeRoot;
    else
        parentItem = static_cast<CTreeItem*>(parent.internalPointer());

    CTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex CMDTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    CTreeItem *childItem = static_cast<CTreeItem*>(index.internalPointer());
    CTreeItem *parentItem = childItem->parentItem();

    if (parentItem == mpTreeRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CMDTreeModel::rowCount(const QModelIndex &parent) const
{
    CTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mpTreeRoot;
    else
        parentItem = static_cast<CTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int CMDTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<CTreeItem*>(parent.internalPointer())->columnCount();
    return mpTreeRoot->columnCount();
}

void CMDTreeModel::setupModelData()
{
    // create Disc node
    CTreeItem*  pDisc  = new CTreeItem({mMDJson["title"].toString(), QString(), QString()}, mpTreeRoot);
    CTreeItem*  pGroup = nullptr;
    QJsonObject currGrp;

    for (const auto& t : mMDJson["tracks"].toArray())
    {
        CTreeItem*  pTrack;
        QJsonObject track      = t.toObject();
        int trackNumber        = track["no"].toInt() + 1;
        QJsonObject trackGroup = group(trackNumber);
        QString trackName      = QString("%1. %2").arg(trackNumber, 2, 10, QChar('0'))
                                        .arg(track["name"].toString());

        if (trackGroup.isEmpty())
        {
            currGrp = trackGroup;

            if (pGroup != nullptr)
            {
                pDisc->appendChild(pGroup);
                pGroup = nullptr;
            }

            pTrack = new CTreeItem({trackName, track["bitrate"].toString(),
                                    track["time"].toString()}, pDisc);

            pDisc->appendChild(pTrack);
        }
        else
        {
            if (trackGroup != currGrp)
            {
                currGrp = trackGroup;

                if (pGroup != nullptr)
                {
                    pDisc->appendChild(pGroup);
                }
                pGroup = new CTreeItem({trackGroup["name"].toString(), QString(), QString()}, pDisc);
            }

            pTrack = new CTreeItem({trackName, track["bitrate"].toString(),
                                    track["time"].toString()}, pGroup);

            pGroup->appendChild(pTrack);
        }
    }

    if (pGroup != nullptr)
    {
        pDisc->appendChild(pGroup);
    }

    mpTreeRoot->appendChild(pDisc);
}

QJsonObject CMDTreeModel::group(int track)
{
    for (const auto& g : mMDJson["groups"].toArray())
    {
        QJsonObject grp = g.toObject();
        if ((track >= grp["first"].toInt()) && (track <= grp["last"].toInt()))
        {
            return grp;
        }
    }
    return QJsonObject();
}

QVariant CMDTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());

    if ((role == Qt::DecorationRole) && (index.column() == 0))
    {
        if (item->parentItem() == mpTreeRoot)
        {
            return QIcon(":/main/md");
        }
        else if (!item->data(2).toString().isEmpty())
        {
            return QIcon(":/view/audio");
        }
        else
        {
            return QIcon(":/view/folder");
        }
    }

    if ((role == Qt::EditRole) && (index.column() == 0))
    {
        return item->data(index.column());
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}

Qt::ItemFlags CMDTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    // name is editable
    if (index.column() == 0)
    {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    return QAbstractItemModel::flags(index);
}

QVariant CMDTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mpTreeRoot->data(section);

    return QAbstractItemModel::headerData(section, orientation, role);
}

//////////////////////////////////////////////////////////////////////////

CTreeItem::CTreeItem(const QVector<QVariant> &data, CTreeItem *parentItem)
    :m_itemData(data), m_parentItem(parentItem)
{

}

CTreeItem::~CTreeItem()
{
    qDeleteAll(m_childItems);
}

void CTreeItem::appendChild(CTreeItem *item)
{
    m_childItems.append(item);
}

CTreeItem *CTreeItem::child(int row)
{
    if ((row < 0) || (row >= m_childItems.size()))
    {
        return nullptr;
    }
    return m_childItems.at(row);
}

int CTreeItem::childCount() const
{
    return m_childItems.count();
}

int CTreeItem::row() const
{
    if (m_parentItem)
    {
        // I don't like const_cast but it doesn't harm here at all
        return m_parentItem->m_childItems.indexOf(const_cast<CTreeItem*>(this));
    }

    return 0;
}

int CTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant CTreeItem::data(int column) const
{
    if ((column < 0) || (column >= m_itemData.size()))
    {
        return QVariant();
    }
    return m_itemData.at(column);
}

CTreeItem *CTreeItem::parentItem()
{
    return m_parentItem;
}
