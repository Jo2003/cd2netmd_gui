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
#include "ccliprocess.h"

class CNetMD : public CCliProcess
{
    Q_OBJECT
#ifdef Q_OS_WIN
    static constexpr const char* NETMD_CLI = "toolchain/netmdcli.exe";
#elif defined Q_OS_MAC
    static constexpr const char* NETMD_CLI = "toolchain/netmdcli";
#else
    // hopefully in path
    static constexpr const char* NETMD_CLI = "netmdcli";
#endif

public:
    enum class NetMDCmd : uint8_t
    {
        DISCINFO,
        WRITE_TRACK_SP,
        WRITE_TRACK_LP2,
        WRITE_TRACK_LP4,
        ADD_GROUP,
        RENAME_DISC,
        RENAME_TRACK,
        RENAME_GROUP,
        DEL_GROUP,
        ERASE_DISC,
        DEL_TRACK,
        UNKNWON
    };

    struct NetMDStartup
    {
        NetMDStartup(NetMDCmd cmd, const QString& trk = "",
                     const QString& title = "", const QString& grp = "",
                     int16_t first = -1, int16_t last = -1, int16_t group = -1)
            : mCmd(cmd), msTrack(trk), msTitle(title), msGroup(grp),
              miFirst(first), miLast(last), miGroup(group)
        {}

        NetMDCmd mCmd;
        QString  msTrack;
        QString  msTitle;
        QString  msGroup;
        int16_t  miFirst;
        int16_t  miLast;
        int16_t  miGroup;
    };

    explicit CNetMD(QObject *parent = nullptr);

    int start(NetMDStartup startup);

private slots:
    void procEnded(int, QProcess::ExitStatus);

signals:
    void jsonOut(QString);

protected:
    NetMDCmd  mCurrCmd;
};
