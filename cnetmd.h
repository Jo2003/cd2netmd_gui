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
#include <netmdcli.h>
#include <cstdio>
#include <QTimer>

//------------------------------------------------------------------------------
//! @brief      This class describes net md handling.
//------------------------------------------------------------------------------
class CNetMD : public QThread
{
    Q_OBJECT

    /// give the child a name
    static constexpr const char * NETMDCLI = "netmdcli";

public:
    static constexpr const char* EMPTY_JSON_RESP = R"({"title": "No disc / NetMD device found!", "otf_enc": 0, "device": "unknown", "trk_count": 0, "disc_flags": "0x00", "t_used": 0, "t_total": 0, "t_free": 0, "groups": [], "tracks": []})";

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
    //--------------------------------------------------------------------------
    void procEnded(bool);
    
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
    //--------------------------------------------------------------------------
    void finished(bool);
    
    //--------------------------------------------------------------------------
    //! @brief      signal thread progress in percent
    //!
    //! @param[in]  <unnamed>  percent
    //--------------------------------------------------------------------------
    void progress(int);

protected:
    /// current job description
    NetMDStartup mCurrJob;
    
    /// file name for json buffer
    QString   mNameFJson;
    
    /// log file name
    QString   mNameFLog;
    
    /// json file pointer
    FILE* mpJsonFile;
    
    /// log file pointer
    FILE* mpLogFile;
    
    /// log file
    QFile mfLog;
    
    /// log content
    QString mLog;
    
    /// cyclic log parce trigger
    QTimer mTReadLog;
};
