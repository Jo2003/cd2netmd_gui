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
#include <QThread>
#include <QFile>
#include <cstdio>
#include <QTimer>
#include <QByteArray>
#include <netmd++.h>
#include <fstream>
#include "ctocmanip.h"

//------------------------------------------------------------------------------
//! @brief      This class describes net md handling.
//------------------------------------------------------------------------------
class CNetMD : public QThread
{
    Q_OBJECT
public:
    static constexpr const char* EMPTY_JSON_RESP = R"({
    "device": "unknown",
    "disc_flags": "0x00",
    "groups": [],
    "otf_enc": 0,
    "sp_upload": 0,
    "toc_manip": 0,
    "t_free": 0,
    "t_total": 0,
    "t_used": 0,
    "title": "No disc / NetMD device found!",
    "tracks": [],
    "trk_count": 0
})";

    /// special marker for TOC edit done + device reset done
    static constexpr int TOCMANIP_DEV_RESET = 999;

    using TocData = CTocManip::TitleVector;

    /// actions to be done on NetMD
    enum class NetMDCmd : uint8_t
    {
        DISCINFO,           ///< get disc info
        WRITE_TRACK_SP,     ///< transfer a track as it is (PCM -> SP, LP2 -> LP2, LP4 -> LP4)
        WRITE_TRACK_LP2,    ///< on the fly transfer / encode PCM -> LP2
        WRITE_TRACK_LP4,    ///< on the fly transfer / encode PCM -> LP4
        ADD_GROUP,          ///< add new group
        RENAME_DISC,        ///< rename MD
        RENAME_TRACK,       ///< rename track 
        RENAME_GROUP,       ///< rename group
        DEL_GROUP,          ///< delete group
        ERASE_DISC,         ///< erase disc
        DEL_TRACK,          ///< delete track
        TOC_MANIP,          ///< TOC manipulation
        UNKNWON             ///< something different
    };

    //--------------------------------------------------------------------------
    //! @brief      startup data structure
    //--------------------------------------------------------------------------
    struct NetMDStartup
    {
        //----------------------------------------------------------------------
        //! @brief      Constructs a new instance.
        //!
        //! @param[in]  cmd    The command
        //! @param[in]  trk    The trk
        //! @param[in]  title  The title
        //! @param[in]  grp    The group
        //! @param[in]  first  The first
        //! @param[in]  last   The last
        //! @param[in]  group  The group
        //----------------------------------------------------------------------
        NetMDStartup(NetMDCmd cmd, const QString& trk = "",
                     const QString& title = "", const QString& grp = "",
                     int16_t first = -1, int16_t last = -1, int16_t group = -1)
            : mCmd(cmd), msTrack(trk), msTitle(title), msGroup(grp),
              miFirst(first), miLast(last), miGroup(group)
        {}

        NetMDCmd mCmd;      ///< command
        QString  msTrack;   ///< track title 
        QString  msTitle;   ///< disc title
        QString  msGroup;   ///< group title
        int16_t  miFirst;   ///< first track
        int16_t  miLast;    ///< last track
        int16_t  miGroup;   ///< group id
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CNetMD(QObject *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    virtual ~CNetMD();

    //--------------------------------------------------------------------------
    //! @brief      store data, start thread
    //!
    //! @param[in]  startup  The startup structure
    //--------------------------------------------------------------------------
    void start(NetMDStartup startup);

    //--------------------------------------------------------------------------
    //! @brief      store data, start thread
    //!
    //! @param[in]  tocData  TOC data for manipulation
    //! @param[in]  resetDev reset device after TOC edit
    //--------------------------------------------------------------------------
    void start(const TocData& tocData, bool resetDev);

    //--------------------------------------------------------------------------
    //! @brief      thread function
    //--------------------------------------------------------------------------
    void run() override;

    //--------------------------------------------------------------------------
    //! @brief      is thread running
    //!
    //! @return     true if running (busy)
    //--------------------------------------------------------------------------
    bool busy();

private slots:
    //--------------------------------------------------------------------------
    //! @brief      the thread ended
    //!
    //! @param[in]  <unnamed>
    //! @param[in]  <unnamed>
    //--------------------------------------------------------------------------
    void procEnded(bool, int);
    
    //--------------------------------------------------------------------------
    //! @brief      extract percent from log
    //--------------------------------------------------------------------------
    void extractPercent();

signals:
    
    //--------------------------------------------------------------------------
    //! @brief      signal json data
    //!
    //! @param[in]  <unnamed>  json data as string
    //--------------------------------------------------------------------------
    void jsonOut(QString);
    
    //--------------------------------------------------------------------------
    //! @brief      signal that thread finished
    //!
    //! @param[in]  <unnamed>  false 
    //! @param[in]  <unnamed>  return value of last action
    //--------------------------------------------------------------------------
    void finished(bool, int);
    
    //--------------------------------------------------------------------------
    //! @brief      signal thread progress in percent
    //!
    //! @param[in]  <unnamed>  percent
    //--------------------------------------------------------------------------
    void progress(int);

protected:

    //--------------------------------------------------------------------------
    //! @brief init the NetMD device
    //--------------------------------------------------------------------------
    void initNetMdDevice();

    //--------------------------------------------------------------------------
    //! @brief      get MD disc info
    //!
    //! @return 0
    //--------------------------------------------------------------------------
    int getDiscInfo();

    //--------------------------------------------------------------------------
    //! @brief write audio track to MD
    //!
    //! @param[in] cmd write command
    //! @param[in] fName file name of source file
    //! @param[in] title track title
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int writeTrack(const NetMDCmd& cmd, const QString& fName, const QString& title);

    //--------------------------------------------------------------------------
    //! @brief add MD group
    //!
    //! @param[in] name group name
    //! @param[in] first first track in group
    //! @param[in] last last track in group
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int addGroup(const QString& name, int first, int last);

    //--------------------------------------------------------------------------
    //! @brief rename MD
    //!
    //! @param[in] name new MD name
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int renameDisc(const QString& name);

    //--------------------------------------------------------------------------
    //! @brief rename MD track
    //!
    //! @param[in] name new MD track name
    //! @param[in] trackNo track number
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int renameTrack(const QString& name, int trackNo);

    //--------------------------------------------------------------------------
    //! @brief rename MD group
    //!
    //! @param[in] name new MD group name
    //! @param[in] groupNo group number
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int renameGroup(const QString& name, int groupNo);

    //--------------------------------------------------------------------------
    //! @brief      erase disc
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int eraseDisc();

    //--------------------------------------------------------------------------
    //! @brief delete MD group
    //!
    //! @param[in] groupNo group number
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int delGroup(int groupNo);

    //--------------------------------------------------------------------------
    //! @brief delete MD track
    //!
    //! @param[in] trackNo track number
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int delTrack(int trackNo);

    //--------------------------------------------------------------------------
    //! @brief do TOC manipulation
    //!
    //! @param[in]  resetDev reset device after TOC edit
    //!
    //! @return 0 -> success; else -> error
    //--------------------------------------------------------------------------
    int doTocManip(bool devReset);

    //--------------------------------------------------------------------------
    //! @brief netmd_time to time_t
    //!
    //! @param[in] t netmd_time
    //!
    //! @return int
    //--------------------------------------------------------------------------
    static inline int toSec(netmd::NetMdTime* t)
    {
        return (t->hour * 3600) + (t->minute * 60) + t->second;
    }

    /// current job description
    NetMDStartup mCurrJob;

    /// data for TOC manipulation
    TocData mTocData;
    
    /// log file name
    QString mNameFLog;
    
    /// log file pointer
    std::ofstream mLogStream;
    
    /// log file
    QFile mfLog;
    
    /// log content
    QString mLog;
    
    /// cyclic log parce trigger
    QTimer mTReadLog;

    /// NetMD device name
    QString mDevName;

    /// NetMD API
    netmd::netmd_pp* mpApi;
};
