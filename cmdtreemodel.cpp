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
#include <QIcon>
#include <string>
#include "helpers.h"
#include "defines.h"

CMDTreeModel::CMDTreeModel(const QString& jsonContent, QObject *parent)
    :QAbstractItemModel(parent)
{
    mMDJson   = nlohmann::json::parse(jsonContent.toStdString());
    mRootData = nlohmann::json::parse(R"(["Name", "Mode", "Time"])");

    // root item with column names
    mpTreeRoot = new CTreeItem(ItemRole::ROOT, mRootData);
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

const CMDTreeModel::SDiscConf *CMDTreeModel::discConf() const
{
    return &mDiscConf;
}

void CMDTreeModel::decreaseFreeTime(time_t secs)
{
    mDiscConf.mFreeTime -= secs;
}

void CMDTreeModel::increaseTracks(int tracks)
{
    mDiscConf.mTrkCount += tracks;
}

nlohmann::json CMDTreeModel::exportJson() const
{
    return mMDJson;
}

void CMDTreeModel::setupModelData()
{
    // fill mDiscConf structure
    if (mMDJson.find("sp_upload") != mMDJson.end())
    {
        mDiscConf.mSPUpload = mMDJson["sp_upload"].get<int>();
    }
    else
    {
        mDiscConf.mSPUpload = 0;
    }

    if (mMDJson.find("toc_manip") != mMDJson.end())
    {
        mDiscConf.mTocManip = mMDJson["toc_manip"].get<int>();
    }
    else
    {
        mDiscConf.mTocManip = 0;
    }

    if (mMDJson.find("otf_enc") != mMDJson.end())
    {
        mDiscConf.mOTFEnc = mMDJson["otf_enc"].get<int>();
    }
    else
    {
        mDiscConf.mOTFEnc = 0;
    }

    if (mMDJson.find("pcm2mono") != mMDJson.end())
    {
        mDiscConf.mPcm2Mono = mMDJson["pcm2mono"].get<int>();
    }
    else
    {
        mDiscConf.mPcm2Mono = 0;
    }

    if (mMDJson.find("trk_count") != mMDJson.end())
    {
        mDiscConf.mTrkCount = mMDJson["trk_count"].get<int>();
    }
    else
    {
        mDiscConf.mTrkCount = 0;
    }

    if (mMDJson.find("t_total") != mMDJson.end())
    {
        mDiscConf.mTotTime = mMDJson["t_total"].get<int>();
    }
    else
    {
        mDiscConf.mTotTime = 0;
    }

    if (mMDJson.find("t_free") != mMDJson.end())
    {
        mDiscConf.mFreeTime = mMDJson["t_free"].get<int>();
    }
    else
    {
        mDiscConf.mFreeTime = 0;
    }

    if (mMDJson.find("t_used") != mMDJson.end())
    {
        mDiscConf.mUsedTime = mMDJson["t_used"].get<int>();
    }
    else
    {
        mDiscConf.mUsedTime = 0;
    }

    if (mMDJson.find("disc_flags") != mMDJson.end())
    {
        mDiscConf.mDiscFlags = std::stoi(mMDJson["disc_flags"].get<std::string>(), nullptr, 16);
    }
    else
    {
        mDiscConf.mDiscFlags = 0;
    }

    if (mMDJson.find("device") != mMDJson.end())
    {
        mDiscConf.mDevice = QString::fromStdString(mMDJson["device"].get<std::string>());
    }
    else
    {
        mDiscConf.mDevice = "No device fetected!";
    }

    // create Disc node
    CTreeItem*  pDisc  = new CTreeItem(ItemRole::DISC, mMDJson, mpTreeRoot);
    CTreeItem*  pGroup = nullptr;
    nlohmann::json currGrp;
    int groupNo = 0;

    if ((mMDJson.find("tracks") != mMDJson.end())
        && mMDJson["tracks"].is_array())
    {
        for (auto& track : mMDJson["tracks"])
        {
            CTreeItem*  pTrack;
            int trackNumber            = track["no"].get<int>() + 1;
            nlohmann::json& trackGroup = group(trackNumber);

            if (trackGroup.empty())
            {
                currGrp = trackGroup;

                if (pGroup != nullptr)
                {
                    pDisc->appendChild(pGroup);
                    pGroup = nullptr;
                }

                pTrack = new CTreeItem(ItemRole::TRACK, track, pDisc);

                pDisc->appendChild(pTrack);
            }
            else
            {
                if (trackGroup != currGrp)
                {
                    if (pGroup != nullptr)
                    {
                        pDisc->appendChild(pGroup);
                    }

                    // add group number (we might need on group rename)
                    trackGroup["no"] = groupNo++;

                    currGrp = trackGroup;

                    pGroup = new CTreeItem(ItemRole::GROUP, trackGroup, pDisc);
                }
                if (pGroup != nullptr)
                {
                    pTrack = new CTreeItem(ItemRole::TRACK, track, pGroup);
                    pGroup->appendChild(pTrack);
                }
            }
        }
    }

    if (pGroup != nullptr)
    {
        pDisc->appendChild(pGroup);
    }

    mpTreeRoot->appendChild(pDisc);
}

nlohmann::json& CMDTreeModel::group(int track)
{
    if ((mMDJson.find("groups") != mMDJson.end())
        && mMDJson["groups"].is_array())
    {
        for (auto& grp : mMDJson["groups"])
        {
            int first = grp["first"].get<int>();
            int last  = grp["last"].get<int>();
            if (last == -1) last = first;
            if ((track >= first) && (track <= last))
            {
                return grp;
            }
        }
    }

    return mEmpty;
}

QVariant CMDTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());

    if ((role == Qt::DecorationRole) && (index.column() == 0))
    {
        if (item->itemRole() == ItemRole::DISC)
        {
            return QIcon(":/main/md");
        }
        else if (item->itemRole() == ItemRole::TRACK)
        {
            return QIcon(":/view/audio");
        }
        else if (item->itemRole() == ItemRole::GROUP)
        {
            return QIcon(":/view/folder");
        }
    }

    if ((role == Qt::EditRole) && (index.column() == 0))
    {
        return item->data(index.column());
    }

    if (role == Qt::DisplayRole)
    {
        if ((item->itemRole() == ItemRole::TRACK) && (index.column() == 0))
        {
            return QString("%1. %2").arg(item->trackNumber(), 2).arg(item->data(index.column()).toString());
        }
        return item->data(index.column());
    }

    return QVariant();
}

bool CMDTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool changed = false;
    if (!index.isValid())
        return changed;

    QString name;

    CTreeItem *item = static_cast<CTreeItem*>(index.internalPointer());
    if ((role == Qt::EditRole) && (index.column() == 0))
    {
        name = value.toString();

        deUmlaut(name);

        switch(item->itemRole())
        {
        case ItemRole::GROUP:
        case ItemRole::TRACK:
            item->rawData()["name"] = name.toStdString();
            changed = true;
            break;
        case ItemRole::DISC:
            item->rawData()["title"] = name.toStdString();
            changed = true;
            break;
        default:
            break;
        }
    }

    if (changed)
    {
        emit editTitle(item->itemRole(), name, item->trackNumber());
    }

    return changed;
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

CTreeItem::CTreeItem(CMDTreeModel::ItemRole role, nlohmann::json &data, CTreeItem *parentItem)
    :mItemData(data), m_parentItem(parentItem), mItRole(role)
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
    return 3;
}

QVariant CTreeItem::data(int column) const
{
    if (mItRole == CMDTreeModel::ItemRole::ROOT)
    {
        if ((column >= 0) && (column < static_cast<int>(mItemData.size())))
        {
            return QString::fromStdString(mItemData.at(column).get<std::string>());
        }
    }
    else if (mItRole == CMDTreeModel::ItemRole::DISC)
    {
        if (column == 0)
        {
            if (mItemData.find("title") != mItemData.end())
            {
                return QString::fromStdString(mItemData["title"].get<std::string>());
            }
        }
    }
    else if (mItRole == CMDTreeModel::ItemRole::TRACK)
    {
        switch(column)
        {
        case 0:
            if (mItemData.find("name") != mItemData.end())
            {
                return QString::fromStdString(mItemData["name"].get<std::string>());
            }
            break;
        case 1:
            if (mItemData.find("bitrate") != mItemData.end())
            {
                return QString::fromStdString(mItemData["bitrate"].get<std::string>());
            }
            break;
        case 2:
            if (mItemData.find("time") != mItemData.end())
            {
                return QString::fromStdString(mItemData["time"].get<std::string>());
            }
            break;
        }
    }
    else if (mItRole == CMDTreeModel::ItemRole::GROUP)
    {
        if (column == 0)
        {
            if (mItemData.find("name") != mItemData.end())
            {
                return QString::fromStdString(mItemData["name"].get<std::string>());
            }
        }
    }

    return QVariant();
}

CTreeItem *CTreeItem::parentItem()
{
    return m_parentItem;
}

CMDTreeModel::ItemRole CTreeItem::itemRole() const
{
    return mItRole;
}

nlohmann::json &CTreeItem::rawData()
{
    return mItemData;
}

int CTreeItem::trackNumber() const
{
    if (mItemData.find("no") != mItemData.end())
    {
        return mItemData["no"].get<int>() + 1;
    }
    return -1;
}
