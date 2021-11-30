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

///
/// \brief The CCDItemModel class
///
class CCDItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    using CDTitles   = QStringList;
    using TrackTimes = QVector<time_t>;

    CCDItemModel() = delete;
    explicit CCDItemModel(const CDTitles& titles, const TrackTimes& times, QObject *parent = nullptr);
    virtual ~CCDItemModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

protected:
    CDTitles   mTitles;
    TrackTimes mTTimes;
};
