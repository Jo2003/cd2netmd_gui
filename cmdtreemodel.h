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
#pragma once
#include <QObject>
#include <QAbstractItemModel>
#include <json.hpp>

class CTreeItem;

//------------------------------------------------------------------------------
//! @brief      This class describes a md tree model.
//------------------------------------------------------------------------------
class CMDTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /// iten role
    enum class ItemRole : uint8_t
    {
        ROOT,       ///< root item
        DISC,       ///< disc item
        GROUP,      ///< group item
        TRACK       ///< track item
    };

    /// disc config
    struct SDiscConf
    {
       int mTocManip;     ///< TOC manipulation support
       int mSPUpload;     ///< SP upload supported
       int mOTFEnc;       ///< On-The-Fly encoding support
       int mPcm2Mono;     ///< PCM2Mono support
       int mNativeMono;   ///< native mono upload support
       int mPCMSpeedUp;   ///< PCM speedup support
       int mTrkCount;     ///< track count
       int mTotTime;      ///< total disc time
       int mFreeTime;     ///< free disc time
       int mUsedTime;     ///< used time
       int mDiscFlags;    ///< disc flags
       int mDevTimeMulti; ///< device time multiplier
       QString mDevice;   ///< device name

       //--------------------------------------------------------------------------
       //! @brief freeTime
       //
       //! @return free disc time
       //--------------------------------------------------------------------------
       int freeTime() const
       {
           return mDevTimeMulti ? (mFreeTime / mDevTimeMulti) : 0;
       }

       //--------------------------------------------------------------------------
       //! @brief usedTime
       //
       //! @return used disc time
       //--------------------------------------------------------------------------
       int usedTime() const
       {
           return mDevTimeMulti ? (mUsedTime / mDevTimeMulti) : 0;
       }

       //--------------------------------------------------------------------------
       //! @brief totalTime
       //
       //! @return total disc time
       //--------------------------------------------------------------------------
       int totalTime() const
       {
           return mDevTimeMulti ? (mTotTime / mDevTimeMulti) : 0;
       }

       //--------------------------------------------------------------------------
       //! @brief addFreeTime
       //
       //! @param[in] val to add
       //
       //! @return result of addition
       //--------------------------------------------------------------------------
       int addFreeTime(int val) const
       {
           return mFreeTime + val * mDevTimeMulti;
       }

       //--------------------------------------------------------------------------
       //! @brief addUsedTime
       //
       //! @param[in] val to add
       //
       //! @return result of addition
       //--------------------------------------------------------------------------
       int addUsedTime(int val) const
       {
           return mUsedTime + val * mDevTimeMulti;
       }
    };

    static constexpr int MD60_SECS = 60 * 60 + 59;
    static constexpr int MD80_SECS = 80 * 60 + 59;

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  jsonContent  The json content
    //! @param      parent       The parent
    //--------------------------------------------------------------------------
    explicit CMDTreeModel(const QString& jsonContent, QObject *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    virtual ~CMDTreeModel();
    
    //--------------------------------------------------------------------------
    //! @brief      get item data
    //!
    //! @param[in]  index  The index
    //! @param[in]  role   The role
    //!
    //! @return     item data
    //--------------------------------------------------------------------------
    QVariant data(const QModelIndex &index, int role) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      Sets the item data.
    //!
    //! @param[in]  index  The index
    //! @param[in]  value  The value
    //! @param[in]  role   The role
    //!
    //! @return     true on success
    //--------------------------------------------------------------------------
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    
    //--------------------------------------------------------------------------
    //! @brief      item flags
    //!
    //! @param[in]  index  The index
    //!
    //! @return     item flags
    //--------------------------------------------------------------------------
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get header data
    //!
    //! @param[in]  section      The section
    //! @param[in]  orientation  The orientation
    //! @param[in]  role         The role
    //!
    //! @return     header data
    //--------------------------------------------------------------------------
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get model index
    //!
    //! @param[in]  row     The row
    //! @param[in]  column  The column
    //! @param[in]  parent  The parent
    //!
    //! @return     model index
    //--------------------------------------------------------------------------
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get parents model index
    //!
    //! @param[in]  index  The index
    //!
    //! @return     model index
    //--------------------------------------------------------------------------
    QModelIndex parent(const QModelIndex &index) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get row count
    //!
    //! @param[in]  parent  The parent
    //!
    //! @return     row count
    //--------------------------------------------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get column count
    //!
    //! @param[in]  parent  The parent
    //!
    //! @return     column count
    //--------------------------------------------------------------------------
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    //--------------------------------------------------------------------------
    //! @brief      export disc config
    //!
    //! @return     pointer to disc config
    //--------------------------------------------------------------------------
    const SDiscConf *discConf() const;
    
    //--------------------------------------------------------------------------
    //! @brief      Decreases the free time.
    //!
    //! @param[in]  secs  The seconds
    //--------------------------------------------------------------------------
    void decreaseFreeTime(time_t secs);
    
    //--------------------------------------------------------------------------
    //! @brief      Increases the tracks.
    //!
    //! @param[in]  tracks  The tracks
    //--------------------------------------------------------------------------
    void increaseTracks(int tracks);
    
    //--------------------------------------------------------------------------
    //! @brief      export json data
    //!
    //! @return     json data
    //--------------------------------------------------------------------------
    nlohmann::json exportJson() const;
    
    //--------------------------------------------------------------------------
    //! @brief      get track group
    //!
    //! @param[in]  track  The track
    //!
    //! @return     group of track
    //--------------------------------------------------------------------------
    nlohmann::json& group(int track);


signals:
    //--------------------------------------------------------------------------
    //! @brief      edit title
    //!
    //! @param[in]  role   The role
    //! @param[in]  title  The title
    //! @param[in]  no     number
    //--------------------------------------------------------------------------
    void editTitle(ItemRole role, QString title, int no);

protected:
    //--------------------------------------------------------------------------
    //! @brief      initialize model from json data
    //--------------------------------------------------------------------------
    void setupModelData();
    
    /// json data
    nlohmann::json mMDJson;
    
    /// root item
    CTreeItem*  mpTreeRoot;
    
    /// root json data
    nlohmann::json mRootData;
    
    /// empty object
    nlohmann::json mEmpty;

    /// disc config
    SDiscConf mDiscConf;
};

//------------------------------------------------------------------------------
//! @brief      This class describes a tree item.
//------------------------------------------------------------------------------
class CTreeItem
{
public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  role        The role
    //! @param      data        The data
    //! @param      parentItem  The parent item
    //--------------------------------------------------------------------------
    explicit CTreeItem(CMDTreeModel::ItemRole role, nlohmann::json& data, CTreeItem *parentItem = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CTreeItem();

    //--------------------------------------------------------------------------
    //! @brief      Appends a child.
    //!
    //! @param      child  The child
    //--------------------------------------------------------------------------
    void appendChild(CTreeItem *child);

    //--------------------------------------------------------------------------
    //! @brief      get the child
    //!
    //! @param[in]  row   The row
    //!
    //! @return     child pointer
    //--------------------------------------------------------------------------
    CTreeItem *child(int row);
    
    //--------------------------------------------------------------------------
    //! @brief      get child count
    //!
    //! @return     child count
    //--------------------------------------------------------------------------
    int childCount() const;
    
    //--------------------------------------------------------------------------
    //! @brief      get column count
    //!
    //! @return     column count
    //--------------------------------------------------------------------------
    int columnCount() const;
    
    //--------------------------------------------------------------------------
    //! @brief      get item data
    //!
    //! @param[in]  column  The column
    //!
    //! @return     item data
    //--------------------------------------------------------------------------
    QVariant data(int column) const;
    
    //--------------------------------------------------------------------------
    //! @brief      get row number
    //!
    //! @return     row number
    //--------------------------------------------------------------------------
    int row() const;
    
    //--------------------------------------------------------------------------
    //! @brief      get parent item
    //!
    //! @return     parent item
    //--------------------------------------------------------------------------
    CTreeItem *parentItem();
    
    //--------------------------------------------------------------------------
    //! @brief      get item role
    //!
    //! @return     item role
    //--------------------------------------------------------------------------
    CMDTreeModel::ItemRole itemRole() const;
    
    //--------------------------------------------------------------------------
    //! @brief      get raw (json) data
    //!
    //! @return     json data
    //--------------------------------------------------------------------------
    nlohmann::json &rawData();
    
    //--------------------------------------------------------------------------
    //! @brief      get track number
    //!
    //! @return     track number
    //--------------------------------------------------------------------------
    int trackNumber() const;

private:
    /// child items
    QVector<CTreeItem*> m_childItems;
    
    /// json item data 
    nlohmann::json& mItemData;
    
    /// parent item
    CTreeItem *m_parentItem;
    
    /// item role
    CMDTreeModel::ItemRole mItRole;
};
