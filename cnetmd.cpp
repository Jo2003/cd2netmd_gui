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
    : QObject(parent), mpNetMDCli(nullptr), mCurrCmd(NetMDCmd::UNKNWON)
{
}

int CNetMD::start(CNetMD::NetMDStartup startup)
{
    int ret = 0;
    mpNetMDCli = new QProcess(this);
    mpNetMDCli->setProcessChannelMode(QProcess::MergedChannels);
    connect(mpNetMDCli, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CNetMD::procEnded);

    QStringList args;
    args << "-v";

    switch(startup.mCmd)
    {
    case NetMDCmd::DISCINFO:
        args << "json_gui";
        break;
    case NetMDCmd::WRITE_TRACK_SP:
        connect(mpNetMDCli, &QProcess::readyRead, this, &CNetMD::readProcOutput);
        args << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::WRITE_TRACK_LP2:
        connect(mpNetMDCli, &QProcess::readyRead, this, &CNetMD::readProcOutput);
        args << "-d" << "lp2" << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::WRITE_TRACK_LP4:
        connect(mpNetMDCli, &QProcess::readyRead, this, &CNetMD::readProcOutput);
        args << "-d" << "lp4" << "send" << startup.msTrack << startup.msTitle;
        break;
    case NetMDCmd::ADD_GROUP:
        args << "add_group" << startup.msGroup << QString::number(startup.miFirst) << QString::number(startup.miLast);
        break;
    default:
        ret = -1;
        break;
    }

    if (ret == 0)
    {
        mCurrCmd = startup.mCmd;
        mpNetMDCli->start(NETMD_CLI, args);
    }

    return ret;
}

int CNetMD::terminate()
{
    if (mpNetMDCli != nullptr)
    {
        mpNetMDCli->terminate();
    }
    return 0;
}

void CNetMD::readProcOutput()
{
}

void CNetMD::procEnded(int iRet, QProcess::ExitStatus ps)
{
    Q_UNUSED(iRet)
    if (ps == QProcess::ExitStatus::NormalExit)
    {
        if (mCurrCmd == NetMDCmd::DISCINFO)
        {
            QString resp = QString::fromUtf8(mpNetMDCli->readAll());

            int start = resp.indexOf(QChar('{'));
            int end   = resp.lastIndexOf(QChar('}'));

            resp = resp.mid(start, 1 + end - start);
            emit jsonOut(resp);
        }
    }

    delete mpNetMDCli;
    mpNetMDCli = nullptr;
}
