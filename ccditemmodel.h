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
#include <QAbstractTableModel>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      This class describes a cd item model.
//------------------------------------------------------------------------------
class CCDItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /// so called typedefs
    using CDTitles   = QStringList;
    using TrackTimes = QVector<time_t>;

    //--------------------------------------------------------------------------
    //! @brief      delete default constructor
    //--------------------------------------------------------------------------
    CCDItemModel() = delete;
    
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  tracks  The audio tracks vector
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CCDItemModel(const c2n::AudioTracks& tracks, QObject *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    virtual ~CCDItemModel();
    
    //--------------------------------------------------------------------------
    //! @brief      get the row count
    //!
    //! @param[in]  parent  The parent
    //!
    //! @return     row count
    //--------------------------------------------------------------------------
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get the column count
    //!
    //! @param[in]  parent  The parent
    //!
    //! @return     column count
    //--------------------------------------------------------------------------
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get item data
    //!
    //! @param[in]  index  The index
    //! @param[in]  role   The role
    //!
    //! @return     item data
    //--------------------------------------------------------------------------
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      Sets the data.
    //!
    //! @param[in]  index  The index
    //! @param[in]  value  The value
    //! @param[in]  role   The role
    //!
    //! @return     true if OK, else error
    //--------------------------------------------------------------------------
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    
    //--------------------------------------------------------------------------
    //! @brief      get item flags
    //!
    //! @param[in]  index  The index
    //!
    //! @return     item flags
    //--------------------------------------------------------------------------
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    
    //--------------------------------------------------------------------------
    //! @brief      get header data
    //!
    //! @param[in]  section      The section
    //! @param[in]  orientation  The orientation
    //! @param[in]  role         The role
    //!
    //! @return     header data
    //--------------------------------------------------------------------------
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

protected:
    CDTitles   mTitles; ///< buffer CD titles
    TrackTimes mTTimes; ///< buffer track times
};
