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
#include <QTimer>
#include <QString>
#include <QFile>
#include <QThread>
#include <thread>
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

    int parseCDText(cdtext_t* pCDT, track_t t, QStringList& ttitles);
    bool busy() const;
    int ripThread(int track, const QString& fName);

public slots:
    bool mediaChanged();
    void getProgress(int percent);
    int cddbReqString();
    void noBusy();

protected:
    CdIo_t* mpCDIO;
    cdrom_drive_t* mpCDAudio;
    cdrom_paranoia_t* mpCDParanoia;
    QTimer mtChkChd;

signals:
    void mediaChgd();
    void progress(int i);
    void match(QStringList l);
    void finished();

private:
    QString mCDDBRequest;
    std::thread*  mpRipThread;
    std::thread*  mpInitThread;
    CCDDB* mpCddb;
    QVector<time_t> mTrackTimes;
    bool mBusy;
};

///
/// \brief The CCDInitThread class
///
class CCDInitThread : public QThread
{
    Q_OBJECT

public:
    CCDInitThread(QObject* parent, CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio, cdrom_paranoia_t** ppCDParanoia);
    void run() override;

protected:
    CdIo_t** mppCDIO;
    cdrom_drive_t** mppCDAudio;
    cdrom_paranoia_t** mppCDParanoia;

signals:
    void finished();
};
