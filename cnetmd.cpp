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
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <libnetmd.h>
#include "cnetmd.h"
#include "defines.h"
#include "helpers.h"

CNetMD::CNetMD(QObject *parent)
    : QThread(parent), mCurrJob(NetMDCmd::UNKNWON), mpLogFile(nullptr),
      mDevList(nullptr)
{
    mNameFLog  = QString("%1/cd2netmd_transfer_log.tmp").arg(QDir::tempPath());
    mTReadLog.setInterval(1000);
    mTReadLog.setSingleShot(false);
    mfLog.setFileName(mNameFLog);

    connect(&mTReadLog, &QTimer::timeout, this, &CNetMD::extractPercent);
    connect(this, &CNetMD::finished, this, &CNetMD::procEnded);
}

CNetMD::~CNetMD()
{
    mTReadLog.stop();
    mfLog.close();

    if (mpLogFile)
    {
        fclose(mpLogFile);
        mpLogFile = nullptr;
    }

    QFile::remove(mNameFLog);
}

void CNetMD::start(NetMDStartup startup)
{
    mLog.clear();
    mCurrJob = startup;
    mfLog.open(QIODevice::Truncate | QIODevice::ReadWrite | QIODevice::Text);
    mTReadLog.start();
    QThread::start();
}

//--------------------------------------------------------------------------
//! @brief get / prepare NetMD device
//!
//! @param[out] md MD header
//!
//! @return nullptr -> error; else NetMD handle
//--------------------------------------------------------------------------
netmd_dev_handle* CNetMD::prepareNetMDDevice(HndMdHdr& md)
{
    netmd_dev_handle* devh;
    netmd_error nmdErr;
    char name[16] = {'\0',};

    md = NULL;

    if (g_LogFilter == c2n::LogLevel::DEBUG)
    {
        netmd_set_log_level(NETMD_LOG_ALL);
    }
    else
    {
        netmd_set_log_level(NETMD_LOG_VERBOSE);
    }

    if ((nmdErr = netmd_init(&mDevList, NULL)) != NETMD_NO_ERROR)
    {
        qCritical() << "Error initializing netmd! " << netmd_strerror(nmdErr);
        return nullptr;
    }

    if (mDevList == NULL)
    {
        qCritical() << "Found no NetMD device(s).";
        return nullptr;
    }

    if ((nmdErr = netmd_open(mDevList, &devh)) != NETMD_NO_ERROR)
    {
        qCritical() << "Error opening netmd! " << netmd_strerror(nmdErr);
        netmd_clean(&mDevList);
        return nullptr;
    }

    if ((nmdErr = netmd_get_devname(devh, name, 16)) != NETMD_NO_ERROR)
    {
        qCritical() << "Could not get device name! " << netmd_strerror(nmdErr);
        netmd_clean(&mDevList);
        netmd_close(devh);
        return nullptr;
    }

    mDevName = name;

    if (netmd_initialize_disc_info(devh, &md) != NETMD_NO_ERROR)
    {
        qCritical() << "Could MD disc info!";
        netmd_clean(&mDevList);
        netmd_close(devh);
        return nullptr;
    }

    return devh;
}

//--------------------------------------------------------------------------
//! @brief free NetMD device
//!
//! @param[in] devh netMD device handle
//! @param[in] pMd  MD header
//--------------------------------------------------------------------------
void CNetMD::freeNetMDDevice(netmd_dev_handle* devh, HndMdHdr* pMd)
{
    free_md_header(pMd);
    netmd_close(devh);
    netmd_clean(&mDevList);
}

//--------------------------------------------------------------------------
//! @brief      get MD disc info
//!
//! @return Json Byte Array
//--------------------------------------------------------------------------
QByteArray CNetMD::getDiscInfo()
{
    qInfo() << "getting MD disc / device info...";
    HndMdHdr md;
    netmd_dev_handle* devh;
    QByteArray ret;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        // Construct JSON object
        QJsonObject tree;

        tree.insert("title", md_header_disc_title(md));
        tree.insert("otf_enc", mDevList->otf_conv);
        tree.insert("device", mDevList->model);
        tree.insert("sp_upload", netmd_dev_supports_sp_upload(devh));

        uint16_t tc = 0;
        if (netmd_request_track_count(devh, &tc) == NETMD_NO_ERROR)
        {
            tree.insert("trk_count", tc);
        }

        uint8_t disc_flags = 0;
        if (netmd_request_disc_flags(devh, &disc_flags) == NETMD_NO_ERROR)
        {
            char hex[5] = {'\0'};
            snprintf(hex, 5, "0x%.02x", disc_flags);
            tree.insert("disc_flags", hex);
        }

        netmd_disc_capacity capacity;
        if (netmd_get_disc_capacity(devh, &capacity) == NETMD_NO_ERROR)
        {
            tree.insert("t_used", toSec(&capacity.recorded));
            tree.insert("t_total", toSec(&capacity.total));
            tree.insert("t_free", toSec(&capacity.available));
        }

        MDGroups* pGroups = md_header_groups(md);
        if (pGroups != NULL)
        {
            QJsonArray groups;
            for (int i = 0; i < pGroups->mCount; i++)
            {
                if (pGroups->mpGroups[i].mFirst > 0)
                {
                    int first = pGroups->mpGroups[i].mFirst;
                    int last  = (pGroups->mpGroups[i].mLast == -1) ? first : pGroups->mpGroups[i].mLast;
                    QJsonObject group;
                    group.insert("name", pGroups->mpGroups[i].mpName);
                    group.insert("first", first);
                    group.insert("last", last);
                    groups.append(group);
                }
            }
            tree.insert("groups", groups);
            md_header_free_groups(&pGroups);
        }

        QJsonArray tracks;
        unsigned char bitrate_id;
        unsigned char flags;
        unsigned char channel;
        char *name, buffer[256];
        struct netmd_track time;
        struct netmd_pair const *trprot, *bitrate;

        trprot = bitrate = 0;

        for(uint16_t i = 0; i < tc; i++)
        {
            QJsonObject track;
            netmd_request_title(devh, i, buffer, 256);
            netmd_request_track_time(devh, i, &time);
            netmd_request_track_flags(devh, i, &flags);
            netmd_request_track_bitrate(devh, i, &bitrate_id, &channel);

            trprot = find_pair(flags, trprot_settings);
            bitrate = find_pair(bitrate_id, bitrates);

            /* Skip 'LP:' prefix... the codec type shows up in the list anyway*/
            if( strncmp( buffer, "LP:", 3 ))
            {
                name = buffer;
            }
            else
            {
                name = buffer + 3;
            }

            // Format track time
            char time_buf[9];
            sprintf(time_buf, "%02i:%02i:%02i", time.minute, time.second, time.tenth);

            // Create JSON track object and add to array
            track.insert("no", i);
            track.insert("protect", trprot->name);
            track.insert("bitrate", bitrate->name);
            track.insert("time", time_buf);
            track.insert("name", name);
            tracks.append(track);
        }
        tree.insert("tracks", tracks);

        QJsonDocument jdoc(tree);
        ret = jdoc.toJson(QJsonDocument::Indented);

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief write audio track to MD
//!
//! @param[in] cmd write command
//! @param[in] fName file name of source file
//! @param[in] title track title
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::writeTrack(const NetMDCmd& cmd, const QString& fName, const QString& title)
{
    qInfo() << "send track '" << title << "' (" << fName << ") to " << mDevName;
    netmd_error ret = NETMD_ERROR;
    unsigned char onTheFlyConvert;
    switch(cmd)
    {
    case NetMDCmd::WRITE_TRACK_LP2:
        onTheFlyConvert = NETMD_DISKFORMAT_LP2;
        break;
    case NetMDCmd::WRITE_TRACK_LP4:
        onTheFlyConvert = NETMD_DISKFORMAT_LP4;
        break;
    case NetMDCmd::WRITE_TRACK_SP:
        onTheFlyConvert = NO_ONTHEFLY_CONVERSION;
        break;
    default:
        qCritical() << "Wrong command given: " << static_cast<int>(cmd);
        return ret;
        break;
    }

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        ret = netmd_send_track(devh,
                               static_cast<const char*>(fName.toUtf8()),
                               static_cast<const char*>(utf8ToMd(title)),
                               onTheFlyConvert);
        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief add MD group
//!
//! @param[in] name group name
//! @param[in] first first track in group
//! @param[in] last last track in group
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::addGroup(const QString& name, int first, int last)
{
    qInfo() << "add group '" << name << "' (first track: " << first << ", last track: " << last << ")";
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        if (md_header_add_group(md, static_cast<const char*>(utf8ToMd(name)), first, last) > 0)
        {
            if (netmd_write_disc_header(devh, md) > 0)
            {
                ret = NETMD_NO_ERROR;
            }
        }

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief rename MD
//!
//! @param[in] name new MD name
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::renameDisc(const QString& name)
{
    qInfo() << "rename MD to '" << name << "'";
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        if (md_header_set_disc_title(md, static_cast<const char*>(utf8ToMd(name))) == 0)
        {
            if (netmd_write_disc_header(devh, md) > 0)
            {
                ret = NETMD_NO_ERROR;
            }
        }

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief rename MD track
//!
//! @param[in] name new MD track name
//! @param[in] trackNo track number
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::renameTrack(const QString& name, int trackNo)
{
    qInfo() << "rename track " << trackNo << " to '" << name << "'";
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        netmd_cache_toc(devh);
        ret = netmd_set_title(devh, trackNo & 0xffff, static_cast<const char*>(utf8ToMd(name))) == 1 ? NETMD_NO_ERROR : NETMD_ERROR;
        netmd_sync_toc(devh);

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief rename MD group
//!
//! @param[in] name new MD group name
//! @param[in] groupNo group number
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::renameGroup(const QString& name, int groupNo)
{
    qInfo() << "rename group " << groupNo << " to '" << name << "'";
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        ret = netmd_set_group_title(devh, md, static_cast<unsigned int>(groupNo),
                                    static_cast<const char*>(utf8ToMd(name))) == 1 ? NETMD_NO_ERROR : NETMD_ERROR;

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief      erase disc
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::eraseDisc()
{
    qInfo() << "erase disc in " << mDevName;
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        ret = netmd_erase_disc(devh) > 0 ? NETMD_NO_ERROR : NETMD_ERROR;

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief delete MD group
//!
//! @param[in] groupNo group number
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::delGroup(int groupNo)
{
    qInfo() << "delete group " << groupNo;
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        ret = netmd_delete_group(devh, md, groupNo & 0xffff) > 0 ? NETMD_NO_ERROR : NETMD_ERROR;

        freeNetMDDevice(devh, &md);
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief delete MD track
//!
//! @param[in] trackNo track number
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::delTrack(int trackNo)
{
    qInfo() << "delete track " << trackNo;
    netmd_error ret = NETMD_ERROR;

    HndMdHdr md;
    netmd_dev_handle* devh;

    if ((devh = prepareNetMDDevice(md)) != nullptr)
    {
        uint16_t tc = 0;
        netmd_request_track_count(devh, &tc);

        if (trackNo < tc)
        {
            netmd_cache_toc(devh);
            netmd_delete_track(devh, trackNo);
            netmd_wait_for_sync(devh);
            netmd_sync_toc(devh);

            if (md_header_del_track(md, trackNo + 1) == 0)
            {
                if (netmd_write_disc_header(devh, md) > 0)
                {
                    ret = NETMD_NO_ERROR;
                }
            }
        }
        freeNetMDDevice(devh, &md);
    }

    return ret;
}

void CNetMD::run()
{
    int ret = 0;

    // log file used by libnetmd
    mpLogFile  = fopen(static_cast<const char*>(mNameFLog.toUtf8()), "a");
    netmd_log_set_fd(mpLogFile);

    switch(mCurrJob.mCmd)
    {
    case NetMDCmd::DISCINFO:
        {
            QByteArray ba = getDiscInfo();
            if (ba.isEmpty())
            {
                ba = EMPTY_JSON_RESP;
            }
            emit jsonOut(static_cast<const char*>(ba));
            qInfo() << static_cast<const char*>(ba);
        }
        break;

    case NetMDCmd::WRITE_TRACK_SP:
    case NetMDCmd::WRITE_TRACK_LP2:
    case NetMDCmd::WRITE_TRACK_LP4:
        ret = writeTrack(mCurrJob.mCmd, mCurrJob.msTrack, mCurrJob.msTitle);
        break;

    case NetMDCmd::ADD_GROUP:
        ret = addGroup(mCurrJob.msGroup, mCurrJob.miFirst, mCurrJob.miLast);
        break;

    case NetMDCmd::RENAME_DISC:
        ret = renameDisc(mCurrJob.msTitle);
        break;

    case NetMDCmd::RENAME_TRACK:
        ret = renameTrack(mCurrJob.msTrack, mCurrJob.miFirst - 1);
        break;

    case NetMDCmd::RENAME_GROUP:
        ret = renameGroup(mCurrJob.msGroup, mCurrJob.miGroup);
        break;

    case NetMDCmd::ERASE_DISC:
        ret = eraseDisc();
        break;

    case NetMDCmd::DEL_GROUP:
        ret = delGroup(mCurrJob.miGroup);
        break;

    case NetMDCmd::DEL_TRACK:
        ret = delTrack(mCurrJob.miFirst);
        break;

    default:
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        qCritical() << "libnetmd action returned with error: " << ret;
    }

    emit finished(false);
}

bool CNetMD::busy()
{
    return QThread::isRunning();
}

void CNetMD::extractPercent()
{
    if (mfLog.isOpen())
    {
        mLog += QString::fromUtf8(mfLog.readAll());
        int pos;

        if ((pos = mLog.lastIndexOf(QChar('%'))) > 0)
        {
            int startPos = mLog.lastIndexOf(QRegExp("[^0-9]+"), pos - 1);

            if (startPos > -1)
            {
                bool ok;
                int length  = (pos - 1) - startPos;
                int percent = mLog.mid(startPos + 1, length).toInt(&ok);

                if (ok)
                {
                    emit progress(percent);
                }
            }
        }
    }
}

void CNetMD::procEnded(bool)
{
    mTReadLog.stop();

    fclose(mpLogFile);
    mpLogFile = nullptr;

    mfLog.close();

    if ((mCurrJob.mCmd == NetMDCmd::ERASE_DISC) || (mCurrJob.mCmd == NetMDCmd::DEL_TRACK))
    {
        start({NetMDCmd::DISCINFO});
    }

    if (!mLog.isEmpty())
    {
        qInfo() << static_cast<const char*>(mLog.toUtf8());
    }
}
