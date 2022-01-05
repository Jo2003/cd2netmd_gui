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
#include <QDir>
#include "defines.h"
#include "helpers.h"

CNetMD::CNetMD(QObject *parent)
    : QThread(parent), mCurrJob(NetMDCmd::UNKNWON), mpJsonFile(nullptr), mpLogFile(nullptr)
{
    mNameFJson = QString("%1/cd2netmd_transfer_json.tmp").arg(QDir::tempPath());
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

    if (mpJsonFile)
    {
        fclose(mpJsonFile);
        mpJsonFile = nullptr;
    }

    if (mpLogFile)
    {
        fclose(mpLogFile);
        mpLogFile = nullptr;
    }

    QFile::remove(mNameFJson);
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

void CNetMD::run()
{
    int ret = 0;
    QStringList args;
    args << "-v";

    switch(mCurrJob.mCmd)
    {
    case NetMDCmd::DISCINFO:
        args << "json_gui";
        break;
    case NetMDCmd::WRITE_TRACK_SP:
        args << "send" << mCurrJob.msTrack << utf8ToMd(mCurrJob.msTitle);
        break;
    case NetMDCmd::WRITE_TRACK_LP2:
        args << "-d" << "lp2" << "send" << mCurrJob.msTrack << utf8ToMd(mCurrJob.msTitle);
        break;
    case NetMDCmd::WRITE_TRACK_LP4:
        args << "-d" << "lp4" << "send" << mCurrJob.msTrack << mCurrJob.msTitle;
        break;
    case NetMDCmd::ADD_GROUP:
        args << "add_group" << utf8ToMd(mCurrJob.msGroup) << QString::number(mCurrJob.miFirst) << QString::number(mCurrJob.miLast);
        break;
    case NetMDCmd::RENAME_DISC:
        args << "rename_disc" << utf8ToMd(mCurrJob.msTitle);
        break;
    case NetMDCmd::RENAME_TRACK:
        args << "rename" << QString::number(mCurrJob.miFirst - 1) << utf8ToMd(mCurrJob.msTrack);
        break;
    case NetMDCmd::RENAME_GROUP:
        args << "retitle" << QString::number(mCurrJob.miGroup) << utf8ToMd(mCurrJob.msGroup);
        break;
    case NetMDCmd::ERASE_DISC:
        args << "erase" << "force";
        break;
    case NetMDCmd::DEL_GROUP:
        args << "deletegroup" << QString::number(mCurrJob.miGroup);
        break;
    case NetMDCmd::DEL_TRACK:
        args << "del_track" << QString::number(mCurrJob.miFirst);
        break;
    default:
        ret = -1;
        break;
    }

    if (ret == 0)
    {
        qInfo() << "netmdcli arguments: " << args;
        mpJsonFile = fopen(static_cast<const char*>(mNameFJson.toUtf8()), "w+");
        mpLogFile  = fopen(static_cast<const char*>(mNameFLog.toUtf8()), "a");

        netmd_cli_set_json_fd(mpJsonFile);
        netmd_cli_set_log_fd(mpLogFile);

        int argc = args.size() + 1;
        char** argv = new char*[argc];
        char** p = argv;

        *p = strdup(NETMDCLI);
        p++;

        for (auto& a : args)
        {
            *p = strdup(static_cast<const char*>(a.toUtf8()));
            p++;
        }

        run_me(argc, argv);

        for (int i = 0; i < argc; i++)
        {
            free(static_cast<void*>(argv[i]));
        }

        delete [] argv;
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

    fclose(mpJsonFile);
    mpJsonFile = nullptr;
    fclose(mpLogFile);
    mpLogFile = nullptr;

    mfLog.close();

    if (mCurrJob.mCmd == NetMDCmd::DISCINFO)
    {
        QFile fjson(mNameFJson);
        if (fjson.open(QIODevice::ReadOnly))
        {
            // mind SHIFT-JIS encoding!
            QString json = mdToUtf8(fjson.readAll());
            int start = json.indexOf(QChar('{'));
            int end   = json.lastIndexOf(QChar('}'));

            if ((start != -1) && (end != -1))
            {
                json = json.mid(start, 1 + end - start);
                emit jsonOut(json);
            }
            else
            {
                emit jsonOut(R"({"title":"no disc found","t_used":0,"t_free":0,"otf_enc":0,"trk_count":0,"t_total":0,"device":"unknown","tracks":[],"groups":[]})");
            }
            qDebug() << json;
        }
    }
    else if ((mCurrJob.mCmd == NetMDCmd::ERASE_DISC) || (mCurrJob.mCmd == NetMDCmd::DEL_TRACK))
    {
        start({NetMDCmd::DISCINFO});
    }

    if (!mLog.isEmpty())
    {
        qDebug() << mLog;
    }
}
