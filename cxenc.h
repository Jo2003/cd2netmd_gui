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
#include <QProcess>

class CXEnc : public QObject
{
    Q_OBJECT
    static constexpr const char* XENC_CLI = "toolchain/netmdcli.exe";

public:
    enum class XEncCmd : uint8_t
    {
        UNKNWON
    };

    struct XEncStartup
    {
        NetMDStartup(NetMDCmd cmd, const QString& trk = "",
                     const QString& title = "", const QString& grp = "",
                     int16_t first = -1, int16_t last = -1, int16_t group = -1)
            : mCmd(cmd), msTrack(trk), msTitle(title), msGroup(grp),
              miFirst(first), miLast(last), miGroup(group)
        {}

        XEncCmd  mCmd;
        QString  msTrack;
        QString  msTitle;
        QString  msGroup;
        int16_t  miFirst;
        int16_t  miLast;
        int16_t  miGroup;
    };

    explicit CXEnc(QObject *parent = nullptr);

    int start(XEncStartup startup);
    int terminate();
    bool busy() const;

private slots:
    void readProcOutput();
    void procEnded(int, QProcess::ExitStatus);

signals:
    void progress(int);

protected:
    QProcess *mpXEncCli;
    XEncCmd   mCurrCmd;
    QString   mResponse;
};
