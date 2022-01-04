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

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CJackTheRipper(QObject *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    virtual ~CJackTheRipper();
    
    //--------------------------------------------------------------------------
    //! @brief      Initializes the object.
    //!
    //! @return     0 -> ok
    //--------------------------------------------------------------------------
    int init();
    
    //--------------------------------------------------------------------------
    //! @brief      cleanup time
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int cleanup();

    //--------------------------------------------------------------------------
    //! @brief      extract audio track(s)
    //!
    //! @param[in]  trackNo   The track no; -1 for all
    //! @param[in]  fName     The file name to store the content
    //! @param[in]  paranoia  use CDDA paranoia if true
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int extractTrack(int trackNo, const QString& fName, bool paranoia);

    //--------------------------------------------------------------------------
    //! @brief      get CDDB pointer
    //!
    //! @return     pointer to internal CDDB class
    //--------------------------------------------------------------------------
    CCDDB *cddb();
    
    //--------------------------------------------------------------------------
    //! @brief      export track times
    //!
    //! @return     track times as vector
    //--------------------------------------------------------------------------
    QVector<time_t> trackTimes();
    
    //--------------------------------------------------------------------------
    //! @brief      get disc length ion seconds
    //!
    //! @return     disc length
    //--------------------------------------------------------------------------
    uint32_t discLength();

    //--------------------------------------------------------------------------
    //! @brief      parse CD Text
    //!
    //! @param      pCDT       The cd text object
    //! @param[in]  firstTrack first track number
    //! @param[in]  lastTrack  nunmber of last track
    //! @param[out] ttitles    The ttitles vector
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int parseCDText(cdtext_t* pCDT, track_t firstTrack, track_t lastTrack, QStringList& ttitles);
    
    //--------------------------------------------------------------------------
    //! @brief      check if ripper is busy
    //!
    //! @return     true if busy
    //--------------------------------------------------------------------------
    bool busy() const;
    
    //--------------------------------------------------------------------------
    //! @brief      thread function for ripping
    //!
    //! @param[in]  track     The track number
    //! @param[in]  fName     The file name
    //! @param[in]  paranoia  The paranoia flag
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int ripThread(int track, const QString& fName, bool paranoia);
    
    //--------------------------------------------------------------------------
    //! @brief      get device info
    //!
    //! @return     The info string.
    //--------------------------------------------------------------------------
    QString deviceInfo();

public slots:

    //--------------------------------------------------------------------------
    //! @brief      check if media was changed
    //!
    //! @return     true if changed
    //--------------------------------------------------------------------------
    bool mediaChanged();
    
    //--------------------------------------------------------------------------
    //! @brief      Gets the progress
    //!
    //! @param[in]  percent  The percent
    //--------------------------------------------------------------------------
    void getProgress(int percent);
    
    //--------------------------------------------------------------------------
    //! @brief      create CDDB request string or extract CD-Text if there
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int cddbReqString();
    
    //--------------------------------------------------------------------------
    //! @brief      set ripper to not busy
    //--------------------------------------------------------------------------
    void noBusy();

protected:
    CdIo_t* mpCDIO;                     ///< CD device pointer
    cdrom_drive_t* mpCDAudio;           ///< CD Audio pointer
    cdrom_paranoia_t* mpCDParanoia;     ///< CD Paranoia pointer
    QTimer mtChkChd;                    ///< check for media change

signals:

    //--------------------------------------------------------------------------
    //! @brief      media was changed
    //--------------------------------------------------------------------------
    void mediaChgd();

    //--------------------------------------------------------------------------
    //! @brief      progress in percent
    //!
    //! @param[in]  i     percent
    //--------------------------------------------------------------------------
    void progress(int i);
    
    //--------------------------------------------------------------------------
    //! @brief      signal match from CDDB
    //!
    //! @param[in]  l     title list
    //--------------------------------------------------------------------------
    void match(QStringList l);
    
    //--------------------------------------------------------------------------
    //! @brief      thread finished
    //--------------------------------------------------------------------------
    void finished();

private:
    QString mCDDBRequest;           ///< CDDB request
    std::thread*  mpRipThread;      ///< rip thread pointer
    CCDDB* mpCddb;                  ///< CDDB pointer
    QVector<time_t> mTrackTimes;    ///< track times buffer
    uint32_t        mDiscLength;    ///< store disc length
    bool mBusy;                     ///< busy flag
};

///
/// \brief The CCDInitThread class
///
class CCDInitThread : public QThread
{
    Q_OBJECT

public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent        The parent
    //! @param      ppCDIO        The pp cdio
    //! @param      ppCDAudio     The pp cd audio
    //! @param      ppCDParanoia  The pp cd paranoia
    //--------------------------------------------------------------------------
    CCDInitThread(QObject* parent, CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio, cdrom_paranoia_t** ppCDParanoia);
    
    //--------------------------------------------------------------------------
    //! @brief      thread function
    //--------------------------------------------------------------------------
    void run() override;

protected:
    CdIo_t** mppCDIO;
    cdrom_drive_t** mppCDAudio;
    cdrom_paranoia_t** mppCDParanoia;

signals:
    //--------------------------------------------------------------------------
    //! @brief      thread finished
    //--------------------------------------------------------------------------
    void finished();
};
