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
#include "cxenc.h"

CXEnc::CXEnc(QObject *parent)
    : QObject(parent), mpXEncCli(nullptr), mCurrCmd(XEncCmd::UNKNWON)
{
}

int CXEnc::start(CNetMD::NetMDStartup startup)
{
    /*
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
        mpNetMDCli->start(NETMD_CLI, args);
    }

    return ret;
    */
    return 0;
}

int CXEnc::terminate()
{
    if (mpXEncCli != nullptr)
    {
        mpXEncCli->terminate();
    }
    return 0;
}

bool CXEnc::busy() const
{
    return mpXEncCli != nullptr;
}

void CXEnc::readProcOutput()
{
    mResponse += mpXEncCli->readAll();
    int pos;

    if ((pos = mResponse.lastIndexOf(QChar('%'))) > 0)
    {
        int startPos = mResponse.lastIndexOf(QRegExp("[^0-9]+"), pos - 1);

        if (startPos > -1)
        {
            bool ok;
            int length  = (pos - 1) - startPos;
            int percent = mResponse.mid(startPos + 1, length).toInt(&ok);

            if (ok)
            {
                emit progress(percent);
            }
        }
    }
}

void CXEnc::procEnded(int iRet, QProcess::ExitStatus ps)
{
    Q_UNUSED(iRet)
    mResponse += QString::fromUtf8(mpXEncCli->readAll());
    if (ps == QProcess::ExitStatus::NormalExit)
    {
        /*
        if (mCurrCmd == NetMDCmd::DISCINFO)
        {
            int start = mResponse.indexOf(QChar('{'));
            int end   = mResponse.lastIndexOf(QChar('}'));

            mResponse = mResponse.mid(start, 1 + end - start);
            emit jsonOut(mResponse);
        }
        */
    }

    qDebug("%s", static_cast<const char*>(mResponse.toUtf8()));

    delete mpXEncCli;
    mpXEncCli = nullptr;
    mResponse.clear();
}
