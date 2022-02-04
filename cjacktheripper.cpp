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
#include "cflac.h"
#include "helpers.h"
#include "defines.h"
#include "caudiotools.h"

///
/// \brief CJackTheRipper::CJackTheRipper
/// \param parent
///
CJackTheRipper::CJackTheRipper(QObject *parent)
    : QObject(parent), mpCDIO(nullptr), mpCDAudio(nullptr),
      mpCDParanoia(nullptr), mpRipThread(nullptr),
      mpCddb(nullptr), mDiscLength(0), mBusy(false), mbCDDB(false),
      mpCopyShop(nullptr)
{
    mpCddb = new CCDDB(this);
}

///
/// \brief CJackTheRipper::~CJackTheRipper
///
CJackTheRipper::~CJackTheRipper()
{
    mtChkChd.stop();

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
    mCueMap.clear();

    cleanup();

    if ((tp == DRIVER_BINCUE) && !name.isEmpty())
    {
        return parseCueFile();
    }
    else
    {
        CCDInitThread *pInit = new CCDInitThread(this, &mpCDIO, &mpCDAudio, &mpCDParanoia, tp, name);

        if (pInit)
        {
            connect(pInit, &CCDInitThread::finished, pInit, &QObject::deleteLater);
            connect(pInit, &CCDInitThread::finished, this, &CJackTheRipper::cddbReqString);
            pInit->start(QThread::LowPriority);
            return 0;
        }
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

    if (mpCopyShop != nullptr)
    {
        mpCopyShop->terminate();
        mpCopyShop->wait();
        delete mpCopyShop;
        mpCopyShop = nullptr;
    }

    if (mpCDParanoia != nullptr)
    {
        cdio_paranoia_free(mpCDParanoia);
        mpCDParanoia = nullptr;
    }

    if (mpCDAudio != nullptr)
    {
        cdio_cddap_close_no_free_cdio(mpCDAudio);
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

    if (mpCopyShop != nullptr)
    {
        mpCopyShop->terminate();
        mpCopyShop->wait();
        delete mpCopyShop;
        mpCopyShop = nullptr;
    }

    if (mCueMap.isEmpty())
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
        mpCopyShop = new CCopyShopThread(this, mCueMap, trackNo, fName);
        if (mpCopyShop != nullptr)
        {
            connect(mpCopyShop, &CCopyShopThread::finished, mpCopyShop, &QObject::deleteLater);
            connect(mpCopyShop, &CCopyShopThread::finished, this, &CJackTheRipper::copyDone);
            connect(mpCopyShop, &CCopyShopThread::progress, this, &CJackTheRipper::getProgress);
            mpCopyShop->start(QThread::NormalPriority);

            mBusy = true;
            return 0;
        }
    }
    return -1;
}

CCDDB *CJackTheRipper::cddb()
{
    return mpCddb;
}

QVector<time_t> CJackTheRipper::trackTimes()
{
    return mTrackTimes;
}

uint32_t CJackTheRipper::discLength()
{
    return mDiscLength;
}

int CJackTheRipper::parseCDText(cdtext_t *pCDT, track_t firstTrack, track_t lastTrack, QStringList &ttitles)
{
    int ret = -1;
    if (pCDT)
    {
        ret = 0;
        const char* tok;
        QString track, performer, title;

        // disc title at index 0
        tok = cdtext_get_const(pCDT, CDTEXT_FIELD_PERFORMER, 0);

        if (tok)
        {
            performer = QString(tok).trimmed();
            track     = QString("%1 - ").arg(performer);
        }

        tok = cdtext_get_const(pCDT, CDTEXT_FIELD_TITLE, 0);
        if (tok)
        {
            title = QString(tok).trimmed();
            if (title.indexOf(performer) != -1)
            {
                track = title;
            }
            else
            {
                track += title;
            }
        }

        if (track.isEmpty())
        {
            track = tr("<untitled>");
        }
        ttitles.append(track);

        // process other tracks
        for (track_t t = firstTrack; t <= lastTrack; t++)
        {
            track = performer = title = "";
            tok   = cdtext_get_const(pCDT, CDTEXT_FIELD_PERFORMER, t);

            if (tok)
            {
                performer = QString(tok).trimmed();
                track     = QString("%1 - ").arg(performer);
            }

            tok = cdtext_get_const(pCDT, CDTEXT_FIELD_TITLE, t);
            if (tok)
            {
                title = QString(tok).trimmed();
                if (title.indexOf(performer) != -1)
                {
                    track = title;
                }
                else
                {
                    track += title;
                }
            }

            if (track.isEmpty())
            {
                track = tr("<untitled>");
            }

            ttitles.append(track);
        }

        qInfo() << "Titles extracted from CD-Text: " << ttitles;
    }
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

        lsn_t   disctStart = cdio_cddap_disc_firstsector(mpCDAudio);
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
            trkStart = disctStart;
            trkEnd   = cdio_cddap_disc_lastsector(mpCDAudio);
        }

        // get track size ...
        size_t trkSz = (trkEnd - trkStart) * CDIO_CD_FRAMESIZE_RAW;
        size_t read  = 0;

        cdio_paranoia_seek(mpCDParanoia, trkStart, SEEK_SET);
        int16_t* pReadBuff;

        QFile f(fName);

        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            CAudioTools::writeWaveHeader(f, trkSz);

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

    if (!mCueMap.isEmpty() && ret.isEmpty())
    {
        ret = "Cue Sheet";
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief      parse cue sheet file if not yet recognized
//!
//! @return 0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CJackTheRipper::parseCueFile()
{
    int ret = -1;
    SCueInfo cueInfo;
    QStringList trackTitles;
    mTrackTimes.clear();
    mDiscLength = 0;
    QFile cuefile(mImgFile);
    QFileInfo fi(mImgFile);
    QString lastSrcFile;

    if (cuefile.open(QIODevice::ReadOnly))
    {
        Cd* cd = cue_parse_string(static_cast<const char*>(cuefile.readAll()));

        if (cd != nullptr)
        {
            ret = 0;

            int noTracks = cd_get_ntrack(cd);
            const char* tok;
            QString track, performer, title;

            // disc title
            Cdtext* cdt = cd_get_cdtext(cd);
            tok = cdtext_get_cue(PTI_PERFORMER, cdt);

            if (tok)
            {
                performer = QString(tok).trimmed();
                track     = QString("%1 - ").arg(performer);
            }

            tok = cdtext_get_cue(PTI_TITLE, cdt);
            if (tok)
            {
                title = QString(tok).trimmed();
                if (title.indexOf(performer) != -1)
                {
                    track = title;
                }
                else
                {
                    track += title;
                }
            }

            if (track.isEmpty())
            {
                track = tr("<untitled>");
            }
            trackTitles.append(track);

            // process other tracks
            for (int i  = 1; i <= noTracks; i++)
            {
                track = performer = title = "";
                Track* t = cd_get_track(cd, i);
                cdt = track_get_cdtext(t);

                long length = track_get_length(t);
                cueInfo.mlStart = track_get_start(t);
                cueInfo.mSrcFileName = QString("%1/%2").arg(fi.absolutePath()).arg(track_get_filename(t));

                if ((lastSrcFile != cueInfo.mSrcFileName) || (length == -1))
                {
                    lastSrcFile = cueInfo.mSrcFileName;

                    QFile fMedia(cueInfo.mSrcFileName);
                    if (fMedia.open(QIODevice::ReadOnly))
                    {
                        size_t audioSz = 0;
                        int flacLength = 0;
                        if (!CAudioTools::checkWaveFile(fMedia, audioSz))
                        {
                            // wave file ...
                            if (length == -1)
                            {
                                long start = track_get_start(t);
                                start *= CDIO_CD_FRAMESIZE_RAW; // CD DA block size
                                length = (audioSz - start) / CDIO_CD_FRAMESIZE_RAW;
                            }
                            cueInfo.mAFormat = TrackAudioFormat::WAVE;
                        }
                        else if(CAudioTools::isFlac(fMedia, flacLength))
                        {
                            // flac file
                            if (length == -1)
                            {
                                long start = track_get_start(t);
                                length = flacLength * CDIO_CD_FRAMES_PER_SEC - start;
                            }
                            cueInfo.mAFormat = TrackAudioFormat::FLAC;
                        }
                        else
                        {
                            // unsupported audio (or whatever)
                            cd_delete(cd);
                            mTrackTimes.clear();
                            mDiscLength = 0;
                            mCueMap.clear();
                            qWarning() << "Unsupported audio format in CUE sheet!";
                            emit match(QStringList() << "Unsupported audio format in CUE sheet!");
                            return -1;
                        }
                    }
                    else
                    {
                        // can't open media file
                        cd_delete(cd);
                        mTrackTimes.clear();
                        mDiscLength = 0;
                        mCueMap.clear();
                        qWarning() << "Can't open media file for CUE sheet!";
                        emit match(QStringList() << "Can't open media file for CUE sheet!");
                        return -1;
                    }
                }

                cueInfo.mlLength = length;
                mCueMap.insert(i, cueInfo);

                mTrackTimes.append(length / CDIO_CD_FRAMES_PER_SEC);
                mDiscLength += length / CDIO_CD_FRAMES_PER_SEC;

                tok = cdtext_get_cue(PTI_PERFORMER, cdt);

                if (tok)
                {
                    performer = QString(tok).trimmed();
                    track     = QString("%1 - ").arg(performer);
                }

                tok = cdtext_get_cue(PTI_TITLE, cdt);
                if (tok)
                {
                    title = QString(tok).trimmed();
                    if (title.indexOf(performer) != -1)
                    {
                        track = title;
                    }
                    else
                    {
                        track += title;
                    }
                }

                // in case no title is stored, use file name
                if (track.isEmpty())
                {
                    track = titleFromFileName({track_get_filename(t)});
                }

                if (track.isEmpty())
                {
                    track = tr("<untitled>");
                }

                qDebug() << i << track << track_get_start(t) << track_get_length(t);

                trackTitles.append(track);
            }

            qInfo() << "Titles extracted from CUE file: " << trackTitles;
            cd_delete(cd);
        }
        else
        {
            qWarning() << "Error parsing CUE file" << mImgFile;
        }
    }
    else
    {
        qWarning() << "Can't open CUE file" << mImgFile;
    }
    emit match(trackTitles);
    return ret;
}

int CJackTheRipper::cddbReqString()
{
    track_t  firstTrack, lastTrack;
    track_t  noTracks;
    uint32_t checkSum = 0, tmp = 0;
    uint32_t offset = 0, lastOffset;
    QVector<lba_t> tracks;
    QStringList trackTitles;
    mTrackTimes.clear();

    mCDDBRequest.clear();

    if (mpCDIO != nullptr)
    {
        // disc length
        int secs   = cdio_get_track_lba(mpCDIO, CDIO_CDROM_LEADOUT_TRACK) / CDIO_CD_FRAMES_PER_SEC;
        noTracks   = cdio_get_num_tracks(mpCDIO);
        firstTrack = cdio_get_first_track_num(mpCDIO);
        lastTrack  = cdio_get_last_track_num(mpCDIO);

        qInfo() << "First track: " << firstTrack << "Last track: " << lastTrack;

        // check for CDText
        parseCDText(cdio_get_cdtext(mpCDIO), firstTrack, lastTrack, trackTitles);

        for (track_t t = firstTrack; t <= lastTrack; t++)
        {
            lastOffset = offset;

            // frame offset
            offset = cdio_get_track_lba(mpCDIO, t);

            tracks.append(offset);

            if (t == firstTrack)
            {
                // first pregap
                secs -= offset / CDIO_CD_FRAMES_PER_SEC;
            }
            else
            {
                mTrackTimes.append((offset - lastOffset) / CDIO_CD_FRAMES_PER_SEC);
            }

            if (trackTitles.isEmpty())
            {
                // checksum
                tmp = offset / CDIO_CD_FRAMES_PER_SEC;
                do
                {
                    checkSum += tmp % 10;
                    tmp /= 10;
                } while (tmp != 0);
            }
        }

        // whole disc length
        mDiscLength = secs;

        // add last track time
        mTrackTimes.append(secs - (offset / CDIO_CD_FRAMES_PER_SEC));

        if (trackTitles.isEmpty() && mbCDDB)
        {
            QString  req;
            uint32_t discId = checkSum << 24 | secs << 8 | noTracks;

            req = QString("%1+%2").arg(discId, 8, 16, QChar('0')).arg(noTracks);

            for (const lba_t& a : tracks)
            {
                req += QString("+%1").arg(a);
            }

            req += QString("+%1").arg(secs);
            mCDDBRequest = req;

            qInfo() << "No CD-Text, do CDDB request: " << req;

            // no CDText -> ask CDDB
            mpCddb->getEntries(req);
        }
        else if (trackTitles.isEmpty() && !mbCDDB)
        {
            qInfo() << "No CD-Text and no CDDB request -> reply empty result!";
            emit match(trackTitles);
        }
        else
        {
            qInfo() << "CD-Text entries found!";
            mCDDBRequest = "";
            emit match(trackTitles);
        }
    }
    else
    {
        emit match(QStringList() << "no disc found!");
    }

    return mCDDBRequest.isEmpty() ? -1 : 0;
}

void CJackTheRipper::noBusy()
{
    mBusy = false;
}

///
/// \brief CJackTheRipper::mediaChanged
/// \return true if media was changed
///
bool CJackTheRipper::mediaChanged()
{
    int ret;
    if (mpCDIO != nullptr)
    {
        ret = cdio_get_media_changed(mpCDIO);
        if (ret == 1)
        {
            qDebug("Media changed!");
            emit mediaChgd();
            return true;
        }
        else if (ret < 0)
        {
            cdio_log(CDIO_LOG_ERROR, "Error: %s",
                     cdio_driver_errmsg(static_cast<driver_return_code_t>(ret)));
        }
    }
    return false;
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
    for (const auto& c : mCueMap)
    {
        if (c.mWavFileName != c.mSrcFileName)
        {
            QFile::remove(c.mWavFileName);
        }
    }
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
            qWarning() << "Can't yet identify CDDA / image!";
        }
    }

    emit finished();
}

////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent        The parent
//! @param      cueMap        ref. to cue map
//! @param      track         The track number
//! @param      fName         The target file name
//--------------------------------------------------------------------------
CCopyShopThread::CCopyShopThread(QObject* parent, CueMap& cueMap, int track, const QString& fName)
    :QThread(parent), mCueMap(cueMap), mTrack(track), mName(fName)
{
}

//--------------------------------------------------------------------------
//! @brief      concatinate audio files
//!
//! @param[in]  sources source audio files
//! @param[in]  srcFormat format of audio files
//! @param[in]  trgFileName name of target file
//!
//! @return 0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CCopyShopThread::conCatWave(const QStringList& sources,
                               const QVector<TrackAudioFormat>& srcFormat,
                               const QString& trgFileName)
{
    int ret = 0;
    QStringList toConCat;
    int percents[] = {0, 25, 50, 75};
    uint8_t percentPos = 0;
    bool done = false;

    auto updPercent = [&]()->void
    {
        if (++percentPos >= (sizeof(percents) / sizeof(int)))
        {
            percentPos = 0;
        }
        emit progress(percents[percentPos]);
    };

    try
    {
        CFlac flac;
        for (int i = 0; i < sources.count(); i++)
        {
            if (srcFormat[i] == TrackAudioFormat::WAVE)
            {
                toConCat << sources[i];
            }
            else
            {
                QString tmpName = QString("%1/cd2netmd_gui_concat_temp_%2.wav").arg(QDir::tempPath()).arg(i);
                toConCat << tmpName;
                qInfo() << "Convert" << sources[i] << "to" << tmpName;
                flac.start(sources[i], tmpName);
                for (int j = 0; j < 150; j++)
                {
                    updPercent();
                    if ((done = flac.waitForFinished(2000)) == true)
                    {
                        break;
                    }
                }

                if (!done)
                {
                    throw std::runtime_error(R"(Can't decode flac in needed time frame.)");
                }
            }
        }

        size_t sz      = 0;
        size_t wholeSz = 0;

        QFile tmpTrg(trgFileName + ".raw");
        if (tmpTrg.open(QIODevice::Truncate | QIODevice::ReadWrite))
        {
            for (const auto& s : toConCat)
            {
                qInfo() << "Append" << s << "to" << tmpTrg.fileName();
                QFile src(s);
                if (src.open(QIODevice::ReadOnly))
                {
                    if (!CAudioTools::stripWaveHeader(src, sz))
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

            QFile trg(trgFileName);
            if (trg.open(QIODevice::WriteOnly))
            {
                qInfo() << "Create" << trgFileName << "from" << tmpTrg.fileName();
                updPercent();
                CAudioTools::writeWaveHeader(trg, wholeSz);
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

        // remove temp files
        for (int i = 0; i < sources.count(); i++)
        {
            if (sources[i] != toConCat[i])
            {
                qDebug() << "Remove temp file" << toConCat[i];
                QFile::remove(toConCat[i]);
            }
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
//! @brief      thread function
//--------------------------------------------------------------------------
void CCopyShopThread::run()
{
    if (mTrack == -1)
    {
        // DaO
        // some audio tracks may share the same audio file
        QStringList sources;
        QVector<TrackAudioFormat> srcFormat;
        for (const auto& c : mCueMap)
        {
            if (!sources.contains(c.mSrcFileName))
            {
                sources << c.mSrcFileName;
                srcFormat.append(c.mAFormat);
            }
        }

        conCatWave(sources, srcFormat, mName);
    }
    else if (mCueMap.contains(mTrack))
    {
        int percents[] = {0, 25, 50, 75};
        uint8_t percentPos = 0;
        bool done = false;

        auto updPercent = [&]()->void
        {
            if (++percentPos >= (sizeof(percents) / sizeof(int)))
            {
                percentPos = 0;
            }
            emit progress(percents[percentPos]);
        };

        SCueInfo& cinfo = mCueMap[mTrack];

        try
        {
            if (cinfo.mAFormat == TrackAudioFormat::WAVE)
            {
                cinfo.mWavFileName = cinfo.mSrcFileName;
            }
            else if (cinfo.mAFormat == TrackAudioFormat::FLAC)
            {
                QFileInfo fi(cinfo.mSrcFileName);
                cinfo.mWavFileName = QString("%1/%2.wav").arg(QDir::tempPath()).arg(fi.baseName());
                qInfo() << "Decode" << cinfo.mSrcFileName << "to" << cinfo.mWavFileName;
                if (!QFile::exists(cinfo.mWavFileName))
                {
                    CFlac flac;
                    flac.start(cinfo.mSrcFileName, cinfo.mWavFileName);
                    for (int j = 0; j < 150; j++)
                    {
                        updPercent();
                        if ((done = flac.waitForFinished(2000)) == true)
                        {
                            break;
                        }
                    }

                    if (!done)
                    {
                        throw std::runtime_error(R"(Can't decode flac in needed time frame.)");
                    }
                }
            }
            updPercent();
            if (CAudioTools::extractRange(cinfo.mWavFileName, mName, cinfo.mlStart, cinfo.mlLength) != 0)
            {
                throw std::runtime_error(R"(Can't extract wave data.)");
            }
        }
        catch (const std::exception& e)
        {
            qInfo() << e.what();
        }
    }
    else
    {
        qWarning() << "Track" << mTrack << "not included in cue file!";
    }

    emit progress(100);
    emit finished();
}
