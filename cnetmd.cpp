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
#include <iomanip>
#include "cnetmd.h"
#include "defines.h"
#include "helpers.h"

CNetMD::CNetMD(QObject *parent)
    : QThread(parent), mCurrJob(NetMDCmd::UNKNWON), mpApi(nullptr)
{
    mpApi = new netmd::netmd_pp;
    mNameFLog  = QString("%1/cd2netmd_transfer_log.tmp").arg(QDir::tempPath());
    mTReadLog.setInterval(1000);
    mTReadLog.setSingleShot(false);
    mfLog.setFileName(mNameFLog);

    if ((mLogStream = std::ofstream(mNameFLog.toStdString(), std::ios_base::out | std::ios_base::trunc)))
    {
        qInfo() << "Log File / Stream" << mNameFLog << "created.";
        mpApi->setLogStream(mLogStream);
    }
    else
    {
        qCritical() << "Can't open Log File / Stream";
    }

    if (!mfLog.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Can't share Log File / Stream";
    }

    connect(&mTReadLog, &QTimer::timeout, this, &CNetMD::extractPercent);
    connect(this, &CNetMD::finished, this, &CNetMD::procEnded);
}

CNetMD::~CNetMD()
{
    mTReadLog.stop();
    mfLog.close();

    if (mLogStream)
    {
        mLogStream.close();
    }

    if (mpApi != nullptr)
    {
        delete mpApi;
    }
}

void CNetMD::start(NetMDStartup startup)
{
    mLog.clear();
    mCurrJob = startup;
    mTReadLog.start();
    QThread::start();
}

//--------------------------------------------------------------------------
//! @brief      store data, start thread
//!
//! @param[in]  tocData  TOC data for manipulation
//--------------------------------------------------------------------------
void CNetMD::start(const TocData& tocData)
{
    mLog.clear();
    mTocData = tocData;
    mCurrJob.mCmd = NetMDCmd::TOC_MANIP;
    mTReadLog.start();
    QThread::start();
}

//--------------------------------------------------------------------------
//! @brief init the NetMD device
//--------------------------------------------------------------------------
void CNetMD::initNetMdDevice()
{
    // set log level for netmd++
    switch(g_LogFilter)
    {
    case c2n::LogLevel::DEBUG:
        mpApi->setLogLevel(netmd::DEBUG);
        break;
    case c2n::LogLevel::INFO:
        mpApi->setLogLevel(netmd::INFO);
        break;
    case c2n::LogLevel::WARN:
        mpApi->setLogLevel(netmd::WARN);
        break;
    case c2n::LogLevel::CRITICAL:
    case c2n::LogLevel::FATAL:
        mpApi->setLogLevel(netmd::CRITICAL);
        break;
    }

    mpApi->initDevice();
    mDevName = QString::fromStdString(mpApi->getDeviceName());
}

//--------------------------------------------------------------------------
//! @brief      get MD disc info
//!
//! @return 0
//--------------------------------------------------------------------------
int CNetMD::getDiscInfo()
{
    using namespace netmd;

    qInfo() << "getting MD disc / device info ...";
    initNetMdDevice();
    int i;
    uint16_t tc = 0;

    std::string s;
    std::ostringstream os;
    
    // helper function to clear std::ostringstream
    auto clrOs = [&]()->void{os.clear(); os.str("");};

    // Construct JSON object
    QJsonObject tree;

    if (mpApi->discTitle(s) == NETMDERR_NO_ERROR)
    {
        if (s.empty())
        {
            s = "<untitled>";
        }
        tree.insert("title", s.c_str());
    }
    tree.insert("otf_enc", mpApi->otfEncodeSupported() ? 1 : 0);
    tree.insert("toc_manip", mpApi->tocManipSupported() ? 1 : 0);
    tree.insert("device", mpApi->getDeviceName().c_str());
    tree.insert("sp_upload", mpApi->spUploadSupported() ? 1 : 0);
    tree.insert("pcm_speedup", mpApi->pcmSpeedupSupported() ? 1 : 0);
    if ((i = mpApi->trackCount()) > -1)
    {
        tree.insert("trk_count", i);
        tc = i;
    }

    if ((i = mpApi->discFlags()) > -1)
    {
        clrOs();
        os << "0x" << std::hex << std::setw(2) << std::setfill('0') << i;
        tree.insert("disc_flags", os.str().c_str());
    }

    DiscCapacity capacity;
    if (mpApi->discCapacity(capacity) == NETMDERR_NO_ERROR)
    {
        tree.insert("t_used", toSec(&capacity.recorded));
        tree.insert("t_total", toSec(&capacity.total));
        tree.insert("t_free", toSec(&capacity.available));
    }

    Groups mdGroups = mpApi->groups();
    QJsonArray groups;
    for (const auto& g : mdGroups)
    {
        if (g.mFirst > 0)
        {
            int first = g.mFirst;
            int last  = (g.mLast == -1) ? first : g.mLast;
            QJsonObject group;
            group.insert("name", g.mName.c_str());
            group.insert("first", first);
            group.insert("last", last);
            groups.append(group);
        }
    }
    tree.insert("groups", groups);

    QJsonArray tracks;
    AudioEncoding aenc;
    TrackProtection tprot;
    TrackTime ttime;
    uint8_t channel;

    for(i = 0; i < tc; i++)
    {
        QJsonObject track;
        s = "";
        tprot = TrackProtection::UNKNOWN;
        aenc  = AudioEncoding::UNKNOWN;
        ttime = {0, 0, 0};
        mpApi->trackTitle(i, s);
        mpApi->trackBitRate(i, aenc, channel);
        mpApi->trackFlags(i, tprot);
        mpApi->trackTime(i, ttime);

        // Create JSON track object and add to array
        track.insert("no", i);

        // protection
        clrOs();
        os << tprot;
        track.insert("protect", os.str().c_str());

        // bitrate
        clrOs();
        os << aenc;
        track.insert("bitrate", os.str().c_str());

        // time
        clrOs();
        os << ttime;
        track.insert("time", os.str().c_str());

        // title
        if (s.empty())
        {
            s = "<untitled>";
        }
        else if (s.find("LP:") == 0)
        {
            s = s.substr(3);
        }

        track.insert("name", s.c_str());
        tracks.append(track);
    }
    tree.insert("tracks", tracks);

    QJsonDocument jdoc(tree);
    QByteArray ba = jdoc.toJson(QJsonDocument::Indented);

    if (ba.isEmpty())
    {
        ba = EMPTY_JSON_RESP;
    }

    emit jsonOut(static_cast<const char*>(ba));
    qInfo() << static_cast<const char*>(ba);

    return 0;
}

//--------------------------------------------------------------------------
//! @brief      enable PCM speedup
//--------------------------------------------------------------------------
void CNetMD::pcmSpeedup()
{
    mpApi->applyPCMSpeedupPatch();
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
    qInfo() << "send track:" << title << "file:" << fName << "to" << mDevName;
    int ret = netmd::NETMDERR_OTHER;
    netmd::DiskFormat onTheFlyConvert;
    switch(cmd)
    {
    case NetMDCmd::WRITE_TRACK_LP2:
        onTheFlyConvert = netmd::NETMD_DISKFORMAT_LP2;
        break;
    case NetMDCmd::WRITE_TRACK_LP4:
        onTheFlyConvert = netmd::NETMD_DISKFORMAT_LP4;
        break;
    case NetMDCmd::WRITE_TRACK_SP:
        onTheFlyConvert = netmd::NO_ONTHEFLY_CONVERSION;
        break;
    default:
        qCritical() << "Wrong command given:" << static_cast<int>(cmd);
        return ret;
        break;
    }

    ret = mpApi->sendAudioFile(fName.toStdString(),
                               static_cast<const char*>(utf8ToMd(title)),
                               onTheFlyConvert);

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
    qInfo() << "add group" << name << "first track:" << first << "last track:" << last << "to" << mDevName;
    return mpApi->createGroup(name.toStdString(), first, last);
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
    qInfo() << "rename MD to" << name << "on" << mDevName;
    return mpApi->setDiscTitle(name.toStdString());
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
    qInfo() << "rename track" << trackNo << "to" << name << "on" << mDevName;
    return mpApi->setTrackTitle(trackNo, name.toStdString());
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
    qInfo() << "rename group" << groupNo << "to" << name << "on" << mDevName;
    return mpApi->setGroupTitle(groupNo, name.toStdString());
}

//--------------------------------------------------------------------------
//! @brief      erase disc
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::eraseDisc()
{
    qInfo() << "erase disc on" << mDevName;
    return mpApi->eraseDisc();
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
    qInfo() << "delete group" << groupNo << "on" << mDevName;
    return mpApi->deleteGroup(groupNo);
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
    qInfo() << "delete track" << trackNo << "on" << mDevName;
    return mpApi->deleteTrack(trackNo);
}

//--------------------------------------------------------------------------
//! @brief do TOC manipulation
//!
//! @return 0 -> success; else -> error
//--------------------------------------------------------------------------
int CNetMD::doTocManip()
{
    CTocManip manip(mpApi);
    return manip.manipulateTOC(mTocData);
}

void CNetMD::run()
{
    int ret = 0;

    switch(mCurrJob.mCmd)
    {
    case NetMDCmd::DISCINFO:
        ret = getDiscInfo();
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

    case NetMDCmd::TOC_MANIP:
        ret = doTocManip();
        break;

    default:
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        qCritical() << "libnetmd action returned with error: " << ret;
    }

    if ((mCurrJob.mCmd == NetMDCmd::ERASE_DISC)
        || (mCurrJob.mCmd == NetMDCmd::DEL_TRACK))
    {
        getDiscInfo();
    }

    emit finished(false, ret);
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

void CNetMD::procEnded(bool, int)
{
    mTReadLog.stop();

    // flush log stream
    mLogStream << std::flush;

    // read all content from log
    if (mfLog.isOpen())
    {
        mLog += QString::fromUtf8(mfLog.readAll());
    }

    if (!mLog.isEmpty())
    {
        qInfo().noquote() << Qt::endl << static_cast<const char*>(mLog.toUtf8());
    }
}
