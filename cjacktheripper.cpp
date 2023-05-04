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
#include "cjacktheripper.h"
#include <QVector>
#include <QFileInfo>
#include <libcue.h>
#include <stdexcept>
#include <QDir>
#include "cffmpeg.h"
#include "helpers.h"
#include "defines.h"

///
/// \brief CJackTheRipper::CJackTheRipper
/// \param parent
///
CJackTheRipper::CJackTheRipper(QObject *parent)
    : QObject(parent), mpCDIO(nullptr), mpCDAudio(nullptr),
      mpCDParanoia(nullptr), mpRipThread(nullptr),
      mpCddb(nullptr), mBusy(false), mbCDDB(false),
      mpFFMpeg(nullptr), miFlacTrack(-99)
#ifdef Q_OS_MAC
      , mpDrUtil(nullptr)
#endif
{
    mpCddb   = new CCDDB(this);
    mpFFMpeg = new CFFMpeg(this);
#ifdef Q_OS_MAC
    mpDrUtil = new CDRUtil(this);
    connect(mpDrUtil, &CDRUtil::fileDone, this, &CJackTheRipper::macCDText);
#endif
    connect(mpFFMpeg, &CFFMpeg::fileDone, this, &CJackTheRipper::extractDone);
    connect(mpFFMpeg, &CFFMpeg::progress, this, &CJackTheRipper::getProgress);
}

///
/// \brief CJackTheRipper::~CJackTheRipper
///
CJackTheRipper::~CJackTheRipper()
{
    // cleanup time
    cleanup();
}

//--------------------------------------------------------------------------
//! @brief      Initializes from CD image / CD drive
//!
//! @param[in]  cddb if true, do cddb request
//! @param[in]  tp driver id type (optional)
//! @param[in]  name image file name (optional)
//!
//! @return     0 -> ok
//--------------------------------------------------------------------------
int CJackTheRipper::init(bool cddb, driver_id_t tp, const QString& name)
{
    mbCDDB   = cddb;
    mDrvId   = tp;
    mImgFile = name;
    mAudioTracks.clear();

    cleanup();

    CCDInitThread *pInit = new CCDInitThread(this, &mpCDIO, &mpCDAudio, &mpCDParanoia, tp, name);

    if (pInit)
    {
        connect(pInit, &CCDInitThread::finished, pInit, &QObject::deleteLater);
        connect(pInit, &CCDInitThread::finished, this, &CJackTheRipper::cddbReqString);
        pInit->start(QThread::LowPriority);
        return 0;
    }

    return -1;
}

int CJackTheRipper::cleanup()
{
    if (mpRipThread != nullptr)
    {
       mpRipThread->join();
       delete mpRipThread;
       mpRipThread = nullptr;
    }

    if (mpCDParanoia != nullptr)
    {
        cdio_paranoia_free(mpCDParanoia);
        mpCDParanoia = nullptr;
    }

    if (mpCDAudio != nullptr)
    {
        cdio_cddap_close_no_free_cdio(mpCDAudio);
        mpCDAudio = nullptr;
    }

    if (mpCDIO != nullptr)
    {
        cdio_destroy(mpCDIO);
        mpCDIO = nullptr;
    }

    return 0;
}

int CJackTheRipper::extractTrack(int trackNo, const QString &fName, bool paranoia)
{
    qInfo("Extract track %d to %s ...", trackNo, static_cast<const char*>(fName.toUtf8()));
    if (mpRipThread != nullptr)
    {
        mpRipThread->join();
        delete mpRipThread;
    }

    if (mAudioTracks.listType() == c2n::AudioTracks::CD)
    {
        mpRipThread = new std::thread(&CJackTheRipper::ripThread, this, trackNo, fName, paranoia);
        if (mpRipThread)
        {
            mBusy = true;
            return 0;
        }
    }
    else
    {
        miFlacTrack = trackNo;
        mFlacFName  = fName;
        extractWave();
        return 0;
    }
    return -1;
}

CCDDB *CJackTheRipper::cddb()
{
    return mpCddb;
}

//--------------------------------------------------------------------------
//! @brief      parse CD Text
//!
//! @param      pCDT       The cd text object
//! @param[in]  track      track number
//! @param[out] ttitle     The track title
//!
//! @return     0 on success
//--------------------------------------------------------------------------
int CJackTheRipper::parseCDText(cdtext_t* pCDT, track_t track, QString& ttitle)
{
    int ret = -1;
    ttitle  = "";

    if (pCDT)
    {
        ret = 0;
        const char* tok;
        QString performer, title;

        tok = cdtext_get_const(pCDT, CDTEXT_FIELD_PERFORMER, track);

        if (tok)
        {
            performer = QString(tok).trimmed();
            ttitle    = QString("%1 - ").arg(performer);
        }

        tok = cdtext_get_const(pCDT, CDTEXT_FIELD_TITLE, track);
        if (tok)
        {
            title = QString(tok).trimmed();
            if (title.indexOf(performer) != -1)
            {
                ttitle = title;
            }
            else
            {
                ttitle += title;
            }
        }
    }

    if (ttitle.isEmpty())
    {
        ttitle = tr("<untitled>");
    }

    qInfo() << "Title" << track << "is named" << ttitle;
    return ret;
}

bool CJackTheRipper::busy() const
{
    return mBusy;
}

int CJackTheRipper::ripThread(int track, const QString &fName, bool paranoia)
{
    int ret = 0;

    try
    {
        if ((mpCDAudio == nullptr) || (mpCDParanoia == nullptr))
        {
            throw std::runtime_error("CD device(s) not initialized!");
        }

        cdio_paranoia_modeset(mpCDParanoia, paranoia ? (PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP) : PARANOIA_MODE_DISABLE);

        // lsn_t   disctStart = cdio_cddap_disc_firstsector(mpCDAudio);
        track_t firstTrack = cdio_get_first_track_num(mpCDIO);
        track_t lastTrack  = cdio_get_last_track_num(mpCDIO);

        lsn_t trkStart = 0;
        lsn_t trkEnd   = 0;

        if (track != -1)
        {
            if (cdio_get_track_format(mpCDIO, track) != TRACK_FORMAT_AUDIO)
            {
                throw std::runtime_error("Track is no audio track!");
            }

            if ((track < firstTrack) || (track > lastTrack))
            {
                throw std::runtime_error("Track is not part of disc!");
            }

            // get track sectors
            trkStart = cdio_cddap_track_firstsector(mpCDAudio, track);
            trkEnd   = cdio_cddap_track_lastsector(mpCDAudio, track);
        }
        else
        {
            // track == -1 -> disc at once mode

            // first track might have data (Mixed Mode)
            if (cdio_get_track_format(mpCDIO, firstTrack) != TRACK_FORMAT_AUDIO)
            {
                // start extraction from next track
                firstTrack++;
            }

            // last track might have data (CD-Plus / CD-Extra)
            if (cdio_get_track_format(mpCDIO, lastTrack) != TRACK_FORMAT_AUDIO)
            {
                // start extraction from next track
                lastTrack--;
            }

            trkStart = cdio_cddap_track_firstsector(mpCDAudio, firstTrack);
            trkEnd   = cdio_cddap_track_lastsector(mpCDAudio, lastTrack);
        }

        // get track size ...
        size_t trkSz = (trkEnd - trkStart) * CDIO_CD_FRAMESIZE_RAW;
        size_t read  = 0;

        cdio_paranoia_seek(mpCDParanoia, trkStart, SEEK_SET);
        int16_t* pReadBuff;

        QFile f(fName);

        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            audio::writeWaveHeader(f, trkSz);

            cdio_cddap_speed_set(mpCDAudio, 8);

            int curPercent = 0, oldPercent = 0;

            while (read < trkSz)
            {
                if((pReadBuff = cdio_paranoia_read(mpCDParanoia, nullptr)) != nullptr)
                {
                    read += CDIO_CD_FRAMESIZE_RAW;
                    f.write((char*)pReadBuff, CDIO_CD_FRAMESIZE_RAW);

                    curPercent = (read * 100) / trkSz;

                    if (curPercent != oldPercent)
                    {
                        emit progress(curPercent);
                        oldPercent = curPercent;
                    }
                }
            }

            f.close();
        }

    }
    catch (const std::exception& e)
    {
        qWarning() << e.what();
        ret = -1;
    }
    noBusy();
    emit finished();
    return ret;
}

//--------------------------------------------------------------------------
//! @brief      copy shop thread ended
//--------------------------------------------------------------------------
void CJackTheRipper::copyDone()
{
    noBusy();
    emit finished();
}

//--------------------------------------------------------------------------
//! @brief      flac extract done
//--------------------------------------------------------------------------
void CJackTheRipper::extractDone()
{
    extractWave();
}

//--------------------------------------------------------------------------
//! @brief      extract to wave
//--------------------------------------------------------------------------
void CJackTheRipper::extractWave()
{
    QFileInfo fi;
    bool startCopy = true;

    if ((miFlacTrack > 0) && (miFlacTrack < mAudioTracks.size()))
    {
        c2n::STrackInfo& ci = mAudioTracks[miFlacTrack];
        if (ci.mWaveFileName.isEmpty())
        {
            if (ci.mConversion)
            {
                fi.setFile(ci.mFileName);
                ci.mWaveFileName = QString("%1/cd2netmd_audio_decode_%2.wav").arg(QDir::tempPath()).arg(fi.baseName());

                if (!QFile::exists(ci.mWaveFileName))
                {
                    mpFFMpeg->start(ci.mFileName, ci.mWaveFileName, ci.mConversion);
                    startCopy = false;
                }
            }
            else
            {
                ci.mWaveFileName = ci.mFileName;
            }
        }
    }
    else if (miFlacTrack == -1)
    {
        // track 0 keeps disc title
        for (int track = 1; track < mAudioTracks.size(); track ++)
        {
            c2n::STrackInfo& ci = mAudioTracks[track];
            if (ci.mWaveFileName.isEmpty())
            {
                if (ci.mConversion)
                {
                    fi.setFile(ci.mFileName);
                    ci.mWaveFileName = QString("%1/cd2netmd_audio_decode_%2.wav").arg(QDir::tempPath()).arg(fi.baseName());

                    if (!QFile::exists(ci.mWaveFileName))
                    {
                        mpFFMpeg->start(ci.mFileName, ci.mWaveFileName, ci.mConversion);
                        startCopy = false;
                        break;
                    }
                }
                else
                {
                    ci.mWaveFileName = ci.mFileName;
                }
            }
        }
    }

    if (startCopy)
    {
        startCopyShop();
    }
}

//--------------------------------------------------------------------------
//! @brief      start file copy stuff
//--------------------------------------------------------------------------
void CJackTheRipper::startCopyShop()
{
    CCopyShopThread* pCopyShop = new CCopyShopThread(this, mAudioTracks, miFlacTrack, mFlacFName);
    if (pCopyShop != nullptr)
    {
        connect(pCopyShop, &CCopyShopThread::finished, pCopyShop, &QObject::deleteLater);
        connect(pCopyShop, &CCopyShopThread::finished, this, &CJackTheRipper::copyDone);
        connect(pCopyShop, &CCopyShopThread::progress, this, &CJackTheRipper::getProgress);
        pCopyShop->start(QThread::NormalPriority);

        mBusy = true;
    }
}

//--------------------------------------------------------------------------
//! @brief      create cddbp query and start request
//--------------------------------------------------------------------------
int CJackTheRipper::cddbRequest()
{
    uint32_t checkSum = 0, tmp = 0;
    QString  req, lbas;
    uint32_t discId = 0;

    for (const auto& t : mAudioTracks)
    {
        if (t.mCDTrackNo > 0)
        {
            // checksum
            tmp = t.mStartLba / CDIO_CD_FRAMES_PER_SEC;
            do
            {
                checkSum += tmp % 10;
                tmp /= 10;
            } while (tmp != 0);

            lbas += QString("+%1").arg(t.mStartLba);
        }
    }

    discId = checkSum << 24
              | (mAudioTracks.at(0).mLbCount / CDIO_CD_FRAMES_PER_SEC) << 8
              | (mAudioTracks.size() - 1);

    req = QString("%1+%2").arg(discId, 8, 16, QChar('0')).arg(mAudioTracks.size() - 1);
    req += lbas;
    req += QString("+%1").arg(mAudioTracks.at(0).mLbCount / CDIO_CD_FRAMES_PER_SEC);
    mCDDBRequest = req;

    qInfo() << "No CD-Text, do CDDB request: " << req;

    // no CDText -> ask CDDB
    mpCddb->getEntries(req, mAudioTracks);

    return 0;
}

QString CJackTheRipper::deviceInfo()
{
    QString ret;
    cdio_hwinfo_t hwInf;

    if (mpCDIO != nullptr)
    {
        if (cdio_get_hwinfo (mpCDIO, &hwInf))
        {
            ret = QString("%1 %2 %3").arg(hwInf.psz_vendor).arg(hwInf.psz_model).arg(hwInf.psz_revision);
            qInfo() << "Found CD Device " << ret;
        }
    }
    else
    {
        ret = mDevInfo;
    }

    return ret;
}

int CJackTheRipper::cddbReqString()
{
    if ((mDrvId == DRIVER_BINCUE) && !mImgFile.isEmpty())
    {
        emit parseCue(mImgFile);
        return 0;
    }

    mAudioTracks.clear();
    mAudioTracks.setListType(c2n::AudioTracks::CD);

    c2n::STrackInfo trackInfo;

    track_t  firstTrack, lastTrack;
    mCDDBRequest = "";

    if (mpCDIO != nullptr)
    {
        firstTrack = cdio_get_first_track_num(mpCDIO);
        lastTrack  = cdio_get_last_track_num(mpCDIO);

        qInfo() << "First track: " << firstTrack << "Last track: " << lastTrack;

        cdtext_t* pCdText = cdio_get_cdtext(mpCDIO);

        trackInfo.mCDTrackNo = 0;
        trackInfo.mStartLba  = 0;
        trackInfo.mTType     = c2n::TrackType::DISC;
        trackInfo.mLbCount   = cdio_get_track_lba(mpCDIO, CDIO_CDROM_LEADOUT_TRACK) - cdio_get_track_lba(mpCDIO, firstTrack);
        parseCDText(pCdText, 0, trackInfo.mTitle);
        mAudioTracks.append(trackInfo);

        for (track_t t = firstTrack; t <= lastTrack; t++)
        {
            trackInfo.mCDTrackNo = t;
            trackInfo.mTType = (cdio_get_track_format(mpCDIO, t) == TRACK_FORMAT_AUDIO) ? c2n::TrackType::AUDIO : c2n::TrackType::DATA;
            trackInfo.mStartLba  = cdio_get_track_lba(mpCDIO, t);
            trackInfo.mLbCount   = cdio_get_track_lba(mpCDIO, (t == lastTrack) ? CDIO_CDROM_LEADOUT_TRACK : t + 1) - trackInfo.mStartLba;
            parseCDText(pCdText, t, trackInfo.mTitle);
            mAudioTracks.append(trackInfo);
        }
#ifdef Q_OS_MAC
        if (!pCdText)
        {
            mpDrUtil->start();
        }
#else
        if (!pCdText && mbCDDB)
        {
            cddbRequest();
        }
        else
        {
            emit match(mAudioTracks);
        }
#endif // Q_OS_MAC
    }

    return mAudioTracks.isEmpty() ? -1 : 0;
}

#ifdef Q_OS_MAC
//--------------------------------------------------------------------------
//! @brief      CD-Text data created by CDRUtil
//!
//! @param[in]  percent  The percent
//--------------------------------------------------------------------------
void CJackTheRipper::macCDText(CDRUtil::CDTextData cdtdata)
{
    if (!cdtdata.isEmpty())
    {
        QString title;
        for (int i = 0; i < mAudioTracks.count(); i++)
        {
            title = "";
            if (i < cdtdata.count())
            {
                if (!cdtdata.at(i).mArtist.isEmpty())
                {
                    title = cdtdata.at(i).mArtist.trimmed() + " - ";
                }

                title += cdtdata.at(i).mTitle.trimmed();
                mAudioTracks[i].mTitle = title;
            }
        }
    }
    else if (mbCDDB)
    {
        cddbRequest();
        return;
    }

    emit match(mAudioTracks);
}
#endif

void CJackTheRipper::noBusy()
{
    mBusy = false;
}

void CJackTheRipper::getProgress(int percent)
{
    emit progress(percent);
}

//--------------------------------------------------------------------------
//! @brief      remove temporary files
//--------------------------------------------------------------------------
void CJackTheRipper::removeTemp()
{
    for (const auto& c : mAudioTracks)
    {
        if (c.mWaveFileName != c.mFileName)
        {
            QFile::remove(c.mWaveFileName);
        }
    }
}

//--------------------------------------------------------------------------
//! @brief      set audio tracks
//!
//! @param[in]  audio tracks vector
//--------------------------------------------------------------------------
void CJackTheRipper::setAudioTracks(const c2n::AudioTracks &tracks)
{
    mAudioTracks = tracks;
}

//--------------------------------------------------------------------------
//! @brief      set device info
//!
//! @param[in]  info device info
//--------------------------------------------------------------------------
void CJackTheRipper::setDeviceInfo(const QString &info)
{
    mDevInfo = info;
}

///////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent        The parent
//! @param      ppCDIO        The pp cdio
//! @param      ppCDAudio     The pp cd audio
//! @param      ppCDParanoia  The pp cd paranoia
//! @param      drv          optional driver type
//! @param      imgFile       optional CD image file
//--------------------------------------------------------------------------
CCDInitThread::CCDInitThread(QObject* parent, CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio,
                             cdrom_paranoia_t** ppCDParanoia, driver_id_t drv, const QString& imgFile)
    :QThread(parent), mppCDIO(ppCDIO), mppCDAudio(ppCDAudio), mppCDParanoia(ppCDParanoia),
     mImgFile(imgFile), mDrvId(drv)
{
}

void CCDInitThread::run()
{
    char **ppsz_cd_drives = nullptr;

    if (mImgFile.isEmpty())
    {
        ppsz_cd_drives = cdio_get_devices_with_cap(nullptr, CDIO_FS_AUDIO, false);
        if (ppsz_cd_drives && *ppsz_cd_drives)
        {
            mImgFile = *ppsz_cd_drives;
            cdio_free_device_list(ppsz_cd_drives);
        }
    }

    if (!mImgFile.isEmpty())
    {
        *mppCDIO    = cdio_open(static_cast<const char*>(mImgFile.toUtf8()), /* mDrvId */DRIVER_UNKNOWN);
        if (*mppCDIO)
        {
            *mppCDAudio = cdio_cddap_identify_cdio(*mppCDIO, CDDA_MESSAGE_FORGETIT, nullptr);

            if (*mppCDAudio)
            {
                if (cdio_cddap_open(*mppCDAudio) == 0)
                {
                    *mppCDParanoia = cdio_paranoia_init(*mppCDAudio);
                }
            }
            else
            {
                qInfo() << "Can't yet identify CDDA / image!";
            }
        }
    }

    emit finished();
}

////////////////////////////////////////////////////////////////////////

const int CCopyShopThread::PERCENTS[] = {0, 25, 50, 75};

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent        The parent
//! @param      cueMap        ref. to cue map
//! @param      track         The track number
//! @param      fName         The target file name
//--------------------------------------------------------------------------
CCopyShopThread::CCopyShopThread(QObject* parent, AudioTracks& cueMap, int track, const QString& fName)
    :QThread(parent), mCueMap(cueMap), mTrack(track), mName(fName), mPercentPos(0)
{
}

//--------------------------------------------------------------------------
//! @brief      concatinate audio files
//!
//! @return 0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CCopyShopThread::conCatWave()
{
    int ret = 0;
    QStringList toConCat;

    try
    {
        size_t sz      = 0;
        size_t wholeSz = 0;

        for (const auto& ci : mCueMap)
        {
            if (!ci.mWaveFileName.isEmpty())
            {
                if (!toConCat.contains(ci.mWaveFileName))
                {
                    toConCat << ci.mWaveFileName;
                }
            }
        }

        if (toConCat.size())
        {
            QFile tmpTrg(mName + ".raw");
            if (tmpTrg.open(QIODevice::Truncate | QIODevice::ReadWrite))
            {
                for (const auto& s : toConCat)
                {
                    qInfo() << "Append" << s << "to" << tmpTrg.fileName();
                    QFile src(s);
                    if (src.open(QIODevice::ReadOnly))
                    {
                        if (!audio::stripWaveHeader(src, sz))
                        {
                            updPercent();
                            tmpTrg.write(src.readAll());
                            wholeSz += sz;
                        }
                        else
                        {
                            throw std::runtime_error(R"(Can't strip wave header.)");
                        }
                    }
                    else
                    {
                        throw std::runtime_error(R"(Can't open wave file.)");
                    }
                }
            }

            QFile trg(mName);
            if (trg.open(QIODevice::WriteOnly))
            {
                qInfo() << "Create" << mName << "from" << tmpTrg.fileName();
                updPercent();
                audio::writeWaveHeader(trg, wholeSz);
                tmpTrg.seek(0);
                trg.write(tmpTrg.readAll());
            }
            else
            {
                throw std::runtime_error(R"(Can't open target file.)");
            }

            tmpTrg.close();
            tmpTrg.remove();
        }
        else
        {
            throw std::runtime_error(R"(Can't open raw target file.)");
        }

        updPercent();
    }
    catch (const std::exception& e)
    {
        qInfo() << e.what();
        ret = -1;
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief      pseudo update progress
//--------------------------------------------------------------------------
void CCopyShopThread::updPercent()
{
    if (++mPercentPos >= (sizeof(PERCENTS) / sizeof(int)))
    {
        mPercentPos = 0;
    }
    emit progress(PERCENTS[mPercentPos]);
}

//--------------------------------------------------------------------------
//! @brief      thread function
//--------------------------------------------------------------------------
void CCopyShopThread::run()
{
    if (mTrack == -1)
    {
        // DaO
        conCatWave();
    }
    else if ((mTrack > 0) && (mTrack < mCueMap.size()))
    {
        STrackInfo& cinfo = mCueMap[mTrack];
        updPercent();
        if (audio::extractRange(cinfo.mWaveFileName, mName, cinfo.mStartLba, cinfo.mLbCount) != 0)
        {
            qInfo() << "Can't extract wave data.";
        }
    }
    else
    {
        qWarning() << "Track" << mTrack << "not included in cue file!";
    }

    emit progress(100);
    emit finished();
}
