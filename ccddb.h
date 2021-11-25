#pragma once
#include <QObject>
#include <QNetworkAccessManager>

class CCDDB : public QObject
{
    Q_OBJECT
    static constexpr const char* CDDB_SERVER = "https://gnudb.gnudb.org:443";

public:
    enum class REQ_WHAT : uint8_t {
        REQ_UNKNOWN,
        REQ_ENTRIES,
        REQ_MATCH
    };

    explicit CCDDB(QObject *parent = nullptr);

    int getEntries(const QString& queryPart);
    int getEntry(const QString& queryPart);

private slots:
    int catchResponse(QNetworkReply *reply);

protected:
    int parseReply(REQ_WHAT type, QString reply);
    QNetworkAccessManager *mpNetwork;

signals:
    void entries(QStringList l);
    void match(QStringList l);
};

