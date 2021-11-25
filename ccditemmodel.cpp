#include "ccditemmodel.h"
#include <QFont>

CCDItemModel::CCDItemModel(const CCDItemModel::CDTitles &titles,
                           const CCDItemModel::TrackTimes &times,
                           QObject *parent)
    :QAbstractTableModel(parent), mTitles(titles), mTTimes(times)
{
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