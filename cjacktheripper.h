#pragma once
#include <QObject>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QThread>
#include <cdio/cdio.h>
#include <cdio/paranoia/paranoia.h>

#include "ccddb.h"

class CRipThread;

///
/// \brief The CJackTheRipper class a CD ripper class able of CD paranoia
///
class CJackTheRipper : public QObject
{
    Q_OBJECT
public:
    explicit CJackTheRipper(QObject *parent = nullptr);
    virtual ~CJackTheRipper();
    virtual int init();
    virtual int cleanup();

    int extractTrack(int trackNo, const QString& fName);

    CCDDB *cddb();

    QVector<time_t> trackTimes();

public slots:
    bool mediaChanged();
    void getProgress(int percent);
    int cddbReqString();

protected:
    CdIo_t* mpCDIO;
    cdrom_drive_t* mpCDAudio;
    cdrom_paranoia_t* mpCDParanoia;
    QTimer mtChkChd;

signals:
    void mediaChgd();
    void progress(int i);

private:
    QString mCDDBRequest;
    CRipThread*  mpRipThread;
    CCDDB* mpCddb;
    QVector<time_t> mTrackTimes;
};

///
/// \brief The CRipThread class
///
class CRipThread : public QThread
{
    Q_OBJECT

public:
    CRipThread(cdrom_drive_t* pCDAudio, cdrom_paranoia_t* pCDParanoia, track_t trkNo, const QString& fileName);
    void run() override;

protected:
    cdrom_drive_t*    mpCDAudio;
    cdrom_paranoia_t* mpCDParanoia;
    track_t           mTrkNo;
    QString           mFileName;

signals:
    void progress(int prct);
    void finished();
};

///
/// \brief The CCDInitThread class
///
class CCDInitThread : public QThread
{
    Q_OBJECT

public:
    CCDInitThread(CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio, cdrom_paranoia_t** ppCDParanoia);
    void run() override;

protected:
    CdIo_t** mppCDIO;
    cdrom_drive_t** mppCDAudio;
    cdrom_paranoia_t** mppCDParanoia;

signals:
    void finished();
};

