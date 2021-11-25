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
