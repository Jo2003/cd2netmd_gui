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
#include <QMap>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QThread>
#include <thread>
#include <cdio/cdio.h>
#include <cdio/logging.h>
#include <cdio/cd_types.h>
#include <cdio/cdtext.h>
#include <cdio/paranoia/paranoia.h>
#ifdef Q_OS_MAC
    #include <cdio/mmc.h>
#endif // mac
#include "cffmpeg.h"
#include "ccddb.h"
#include "audio.h"

class CCopyShopThread;

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
    //! @brief      Initializes from CD image / CD drive
    //!
    //! @param[in]  cddb if true, do cddb request
    //! @param[in]  tp driver id type (optional)
    //! @param[in]  name image file name (optional)
    //!
    //! @return     0 -> ok
    //--------------------------------------------------------------------------
    int init(bool cddb, driver_id_t tp = DRIVER_UNKNOWN, const QString& name = QString());
    
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
    //! @brief      parse CD Text
    //!
    //! @param      pCDT       The cd text object
    //! @param[in]  track      track number
    //! @param[out] ttitle     The track title
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int parseCDText(cdtext_t* pCDT, track_t track, QString& ttitle);
    
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

    //--------------------------------------------------------------------------
    //! @brief      remove temporary files
    //--------------------------------------------------------------------------
    void removeTemp();

    //--------------------------------------------------------------------------
    //! @brief      set audio tracks
    //!
    //! @param[in]  audio tracks vector
    //--------------------------------------------------------------------------
    void setAudioTracks(const c2n::AudioTracks& tracks);

    //--------------------------------------------------------------------------
    //! @brief      set device info
    //!
    //! @param[in]  info device info
    //--------------------------------------------------------------------------
    void setDeviceInfo(const QString& info);

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

    //--------------------------------------------------------------------------
    //! @brief      copy shop thread ended
    //--------------------------------------------------------------------------
    void copyDone();

    //--------------------------------------------------------------------------
    //! @brief      flac extract done
    //--------------------------------------------------------------------------
    void extractDone();

protected:
    //--------------------------------------------------------------------------
    //! @brief      extract to Wave
    //--------------------------------------------------------------------------
    void extractWave();

    //--------------------------------------------------------------------------
    //! @brief      start file copy stuff
    //--------------------------------------------------------------------------
    void startCopyShop();

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
    //! @param[in]  tracks AudioTracks vector
    //--------------------------------------------------------------------------
    void match(c2n::AudioTracks tracks);
    
    //--------------------------------------------------------------------------
    //! @brief      thread finished
    //--------------------------------------------------------------------------
    void finished();

    //--------------------------------------------------------------------------
    //! @brief      tell main window to parse cue file
    //!
    //! @param[in]  cue file name
    //--------------------------------------------------------------------------
    void parseCue(QString fileName);

private:
    QString mCDDBRequest;           ///< CDDB request
    std::thread*  mpRipThread;      ///< rip thread pointer
    CCDDB* mpCddb;                  ///< CDDB pointer
    bool mBusy;                     ///< busy flag
    bool mbCDDB;
    QString mImgFile;
    driver_id_t mDrvId = DRIVER_UNKNOWN;
    CFFMpeg* mpFFMpeg;
    int miFlacTrack;
    QString mFlacFName;
    c2n::AudioTracks mAudioTracks;
    QString mDevInfo;
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
    //! @param      drv           optional driver type
    //! @param      imgFile       optional CD image file name
    //--------------------------------------------------------------------------
    CCDInitThread(QObject* parent,
                  CdIo_t** ppCDIO,
                  cdrom_drive_t** ppCDAudio,
                  cdrom_paranoia_t** ppCDParanoia,
                  driver_id_t drv = DRIVER_UNKNOWN,
                  const QString& imgFile = QString());
    
    //--------------------------------------------------------------------------
    //! @brief      thread function
    //--------------------------------------------------------------------------
    void run() override;

protected:
    CdIo_t** mppCDIO;
    cdrom_drive_t** mppCDAudio;
    cdrom_paranoia_t** mppCDParanoia;
    QString mImgFile;
    driver_id_t mDrvId;

signals:
    //--------------------------------------------------------------------------
    //! @brief      thread finished
    //--------------------------------------------------------------------------
    void finished();
};

//--------------------------------------------------------------------------
//! @brief      thread class for cue copy actions
//--------------------------------------------------------------------------
class CCopyShopThread : public QThread
{
    Q_OBJECT

    using AudioTracks = c2n::AudioTracks;
    using STrackInfo  = c2n::STrackInfo;

    static const int PERCENTS[];

public:

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent        The parent
    //! @param      cueMap        ref. to cue map
    //! @param      track         The track number
    //! @param      fName         The target file name
    //--------------------------------------------------------------------------
    CCopyShopThread(QObject* parent, AudioTracks& cueMap, int track, const QString& fName);

    //--------------------------------------------------------------------------
    //! @brief      thread function
    //--------------------------------------------------------------------------
    void run() override;

signals:
    //--------------------------------------------------------------------------
    //! @brief      thread finished
    //--------------------------------------------------------------------------
    void finished();

    //--------------------------------------------------------------------------
    //! @brief      progress in percent
    //--------------------------------------------------------------------------
    void progress(int);

protected:

    //--------------------------------------------------------------------------
    //! @brief      concatinate audio files
    //!
    //! @return 0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int conCatWave();

    //--------------------------------------------------------------------------
    //! @brief      pseudo update progress
    //--------------------------------------------------------------------------
    void updPercent();

    AudioTracks& mCueMap;
    int mTrack;
    QString mName;
    uint8_t mPercentPos;
};
