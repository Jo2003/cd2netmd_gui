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
#include <cdio/logging.h>
#include <cdio/cd_types.h>
#include <cdio/cdtext.h>
#include <QVector>
#include <stdexcept>
#include "helpers.h"
#include "defines.h"

static int writeFileHeader(QFile &wf, size_t byteCount)
{
    wf.write("RIFF", 4);
    putNum(byteCount + 44 - 8, wf, 4);
    wf.write("WAVEfmt ", 8);
    putNum(16, wf, 4);
    putNum(1, wf, 2);
    putNum(2, wf, 2);
    putNum(44100, wf, 4);
    putNum(44100 * 2 * 2, wf, 4);
    putNum(4, wf, 2);
    putNum(16, wf, 2);
    wf.write("data", 4);
    putNum(byteCount, wf, 4);

    return 0;
}

///
/// \brief CJackTheRipper::CJackTheRipper
/// \param parent
///
CJackTheRipper::CJackTheRipper(QObject *parent)
    : QObject(parent), mpCDIO(nullptr), mpCDAudio(nullptr),
      mpCDParanoia(nullptr), mpRipThread(nullptr),
      mpCddb(nullptr), mDiscLength(0), mBusy(false)
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

///
/// \brief CJackTheRipper::init
/// \return 0 -> ok; -1 -> error
///
int CJackTheRipper::init()
{
    cleanup();
    CCDInitThread *pInit = new CCDInitThread(this, &mpCDIO, &mpCDAudio, &mpCDParanoia);

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

    mpRipThread = new std::thread(&CJackTheRipper::ripThread, this, trackNo, fName, paranoia);

    if (mpRipThread)
    {
        mBusy = true;
        return 0;
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
            writeFileHeader(f, trkSz);

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

        if (trackTitles.isEmpty())
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

///////////////////////////////////////////////////////////////////////////////////

CCDInitThread::CCDInitThread(QObject *parent, CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio, cdrom_paranoia_t** ppCDParanoia)
    :QThread(parent), mppCDIO(ppCDIO), mppCDAudio(ppCDAudio), mppCDParanoia(ppCDParanoia)
{
}

void CCDInitThread::run()
{
    char **ppsz_cd_drives = cdio_get_devices_with_cap(nullptr, CDIO_FS_AUDIO, false);

    if (ppsz_cd_drives && *ppsz_cd_drives)
    {
        *mppCDIO    = cdio_open(*ppsz_cd_drives, DRIVER_UNKNOWN);
        *mppCDAudio = cdio_cddap_identify_cdio(*mppCDIO, CDDA_MESSAGE_FORGETIT, nullptr);

        cdio_free_device_list(ppsz_cd_drives);

        if (cdio_cddap_open(*mppCDAudio) == 0)
        {
            *mppCDParanoia = cdio_paranoia_init(*mppCDAudio);
        }
    }

    emit finished();
}
