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
    : CCliProcess(parent), mCurrCmd(XEncCmd::NONE)
{
}

int CXEnc::start(XEncCmd cmd, const QString& tmpFileName)
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

int CXEnc::atrac3WaveHeader(const QString &tmpFileName, CXEnc::XEncCmd cmd, uint32_t dataSz)
{
    /*
    int ret = -1;

    if ((fWave != nullptr)
        && ((mode == NetMDCmds::WRITE_TRACK_LP2) || (mode == NetMDCmds::WRITE_TRACK_LP4))
        && (dataSz > 92)) // 1x lp4 frame size
    {
        // heavily inspired by atrac3tool and completed through
        // reverse engineering of ffmpeg output ...
        char dstFormatBuf[0x20];
        WAVEFORMATEX *pDstFormat   = (WAVEFORMATEX *)dstFormatBuf;
        pDstFormat->wFormatTag     = WAVE_FORMAT_SONY_SCX;
        pDstFormat->nChannels      = 2;
        pDstFormat->nSamplesPerSec = 44100;
        if (mode == NetMDCmds::WRITE_TRACK_LP2)
        {
            pDstFormat->nAvgBytesPerSec = 16537;
            pDstFormat->nBlockAlign     = 0x180;
            memcpy(&dstFormatBuf[0x12], "\x01\x00\x44\xAC\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00", 0xE);
        }
        else if (mode == NetMDCmds::WRITE_TRACK_LP4)
        {
            pDstFormat->nAvgBytesPerSec = 8268;
            pDstFormat->nBlockAlign     = 0xc0;
            memcpy(&dstFormatBuf[0x12], "\x01\x00\x44\xAC\x00\x00\x01\x00\x01\x00\x01\x00\x00\x00", 0xE);
        }

        pDstFormat->wBitsPerSample = 0;
        pDstFormat->cbSize         = 0xE;

        uint32_t i = 0xC + 8 + 0x20 + 8 + dataSz - 8;

        fwrite("RIFF", 1, 4, fWave);
        fwrite(&i, 4, 1, fWave);
        fwrite("WAVE", 1, 4, fWave);

        fwrite("fmt ", 1, 4, fWave);
        i = 0x20;
        fwrite(&i, 4, 1, fWave);
        fwrite(dstFormatBuf, 1, 0x20, fWave);

        fwrite("data", 1, 4, fWave);
        i = dataSz;
        fwrite(&i, 4, 1, fWave);

        ret = 0;
    }

    return ret;
    */
    return 0;
}
