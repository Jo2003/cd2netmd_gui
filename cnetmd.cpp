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
#include "cnetmd.h"

CNetMD::CNetMD(QObject *parent)
    : CCliProcess(parent), mCurrCmd(NetMDCmd::UNKNWON)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CNetMD::procEnded);
}

int CNetMD::start(CNetMD::NetMDStartup startup)
{
    int ret = 0;
    QStringList args;
    args << "-v";

    switch(startup.mCmd)
    {
    case NetMDCmd::DISCINFO:
        args << "json_gui";
        break;
    case NetMDCmd::WRITE_TRACK_SP:
        args << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::WRITE_TRACK_LP2:
        args << "-d" << "lp2" << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::WRITE_TRACK_LP4:
        args << "-d" << "lp4" << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::ADD_GROUP:
        args << "add_group" << startup.msGroup << QString::number(startup.miFirst) << QString::number(startup.miLast);
        break;
    case NetMDCmd::RENAME_DISC:
        args << "rename_disc" << startup.msTitle;
        break;
    case NetMDCmd::RENAME_TRACK:
        args << "rename" << QString::number(startup.miFirst - 1) << startup.msTrack;
        break;
    case NetMDCmd::RENAME_GROUP:
        args << "retitle" << QString::number(startup.miGroup) << startup.msGroup;
        break;
    default:
        ret = -1;
        break;
    }

    if (ret == 0)
    {
        mCurrCmd = startup.mCmd;
        run(NETMD_CLI, args);
    }

    return ret;
}

void CNetMD::procEnded(int iRet, QProcess::ExitStatus ps)
{
    Q_UNUSED(iRet)
    mLog += QString::fromUtf8(readAllStandardOutput());
    if (ps == QProcess::ExitStatus::NormalExit)
    {
        if (mCurrCmd == NetMDCmd::DISCINFO)
        {
            int start = mLog.indexOf(QChar('{'));
            int end   = mLog.lastIndexOf(QChar('}'));

            mLog = mLog.mid(start, 1 + end - start);
            emit jsonOut(mLog);
        }
    }

    qDebug("%s", static_cast<const char*>(mLog.toUtf8()));
}
