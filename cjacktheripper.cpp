#include "cjacktheripper.h"
#include <cdio/logging.h>
#include <cdio/cd_types.h>
#include <QVector>
#include <stdexcept>

static int putNum(uint32_t num, QFile &f, size_t sz)
{
    unsigned int i;
    unsigned char c;

    for (i = 0; sz--; i++)
    {
        c = (num >> (i << 3)) & 0xff;
        if (f.write((char*)&c, 1) != 1)
        {
            return -1;
        }
    }
    return 0;
}

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
      mpCDParanoia(nullptr), mpRipThread(nullptr), mpCddb(nullptr)
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
    CCDInitThread *pInit = new CCDInitThread(&mpCDIO, &mpCDAudio, &mpCDParanoia);

    if (pInit)
    {
        connect(pInit, &CCDInitThread::finished, pInit, &QObject::deleteLater);
        connect(pInit, &CCDInitThread::finished, this, &CJackTheRipper::cddbReqString);
        pInit->run();
        return 0;
    }

    return -1;
}

int CJackTheRipper::cleanup()
{
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

int CJackTheRipper::extractTrack(int trackNo, const QString &fName)
{
    mpRipThread = new CRipThread(mpCDAudio, mpCDParanoia, trackNo, fName);

    if (mpRipThread)
    {
        connect(mpRipThread, &CRipThread::finished, mpRipThread, &QObject::deleteLater);
        connect(mpRipThread, &CRipThread::progress, this, &CJackTheRipper::getProgress);

        mpRipThread->start();

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

int CJackTheRipper::cddbReqString()
{
    QString req;

    track_t  firstTrack;
    track_t  noTracks;
    uint32_t checkSum = 0, tmp = 0;
    uint32_t offset = 0, lastOffset;
    QVector<lba_t> tracks;
    mTrackTimes.clear();

    mCDDBRequest.clear();

    if (mpCDIO != nullptr)
    {
        // disc length
        int secs   = cdio_get_track_lba(mpCDIO, CDIO_CDROM_LEADOUT_TRACK) / CDIO_CD_FRAMES_PER_SEC;
        noTracks   = cdio_get_num_tracks(mpCDIO);
        firstTrack = cdio_get_first_track_num(mpCDIO);

        for (int j = 0; j < noTracks; j++)
        {
            lastOffset = offset;

            // frame offset
            offset = cdio_get_track_lba(mpCDIO, j + firstTrack);

            tracks.append(offset);

            if (j == 0)
            {
                // first pregap
                secs -= offset / CDIO_CD_FRAMES_PER_SEC;
            }
            else
            {
                mTrackTimes.append((offset - lastOffset) / CDIO_CD_FRAMES_PER_SEC);
            }

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

        // add last track time
        mTrackTimes.append(secs - (offset / CDIO_CD_FRAMES_PER_SEC));

        uint32_t discId = checkSum << 24 | secs << 8 | noTracks;

        req = QString("%1+%2").arg(discId, 8, 16, QChar('0')).arg(noTracks);

        for (const lba_t& a : tracks)
        {
            req += QString("+%1").arg(a);
        }

        req += QString("+%1").arg(secs);
        mCDDBRequest = req;

        mpCddb->getEntries(req);
    }

    return mCDDBRequest.isEmpty() ? -1 : 0;
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

CRipThread::CRipThread(cdrom_drive_t *pCDAudio, cdrom_paranoia_t *pCDParanoia,
                       track_t trkNo, const QString &fileName)
    :QThread(), mpCDAudio(pCDAudio), mpCDParanoia(pCDParanoia),
     mTrkNo(trkNo), mFileName(fileName)
{
}

void CRipThread::run()
{
    try
    {
        if ((mpCDAudio == nullptr) || (mpCDParanoia == nullptr))
        {
            throw std::runtime_error("CD device(s) not initialized!");
        }

        cdio_paranoia_modeset(mpCDParanoia, PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP);

        lsn_t disctStart   = cdio_cddap_disc_firstsector(mpCDAudio);
        track_t firstTrack = cdio_cddap_sector_gettrack(mpCDAudio, disctStart);
        track_t trackCount = cdio_cddap_tracks(mpCDAudio);

        if ((mTrkNo < firstTrack) || (mTrkNo > (firstTrack + (trackCount - 1))))
        {
            throw std::runtime_error("Track is not part of disc!");
        }

        // get track sectors
        lsn_t trkStart = cdio_cddap_track_firstsector(mpCDAudio, mTrkNo);
        lsn_t trkEnd   = cdio_cddap_track_lastsector(mpCDAudio, mTrkNo);

        // get track size ...
        size_t trkSz = (trkEnd - trkStart) * CDIO_CD_FRAMESIZE_RAW;
        size_t read  = 0;

        cdio_paranoia_seek(mpCDParanoia, trkStart, SEEK_SET);
        int16_t* pReadBuff;

        QFile f(mFileName);

        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            writeFileHeader(f, trkSz);

            cdio_cddap_speed_set(mpCDAudio, 4);

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
        qDebug("%s", e.what());
    }

    emit finished();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

CCDInitThread::CCDInitThread(CdIo_t** ppCDIO, cdrom_drive_t** ppCDAudio, cdrom_paranoia_t** ppCDParanoia)
    :QThread(), mppCDIO(ppCDIO), mppCDAudio(ppCDAudio), mppCDParanoia(ppCDParanoia)
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
