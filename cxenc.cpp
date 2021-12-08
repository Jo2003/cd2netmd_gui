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
#include <QFileInfo>
#include "cxenc.h"
#include "utils.h"

CXEnc::CXEnc(QObject *parent)
    : CCliProcess(parent), mCurrCmd(XEncCmd::NONE)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CXEnc::finishCopy);
}

int CXEnc::start(XEncCmd cmd, const QString& tmpFileName)
{
    QStringList params;
    mCurrCmd       = cmd;
    mAtracFileName = tmpFileName + ".aea";

    switch (cmd)
    {
    case XEncCmd::LP2_ENCODE:
        params << "-e" << "atrac3" << "--bitrate=128" << "-i" << tmpFileName << "-o" << mAtracFileName;
        break;
    case XEncCmd::LP4_ENCODE:
        params << "-e" << "atrac3" << "--bitrate=64" << "-i" << tmpFileName << "-o" << mAtracFileName;
        break;
    default:
        return -1;
    }

    run(XENC_CLI, params);
    return 0;
}

int CXEnc::atrac3WaveHeader(QFile& waveFile, XEncCmd cmd, size_t dataSz)
{
    int ret = -1;

    if ((waveFile.isOpen())
        && ((cmd == XEncCmd::LP2_ENCODE) || (cmd == XEncCmd::LP4_ENCODE))
        && (dataSz > 92)) // 1x lp4 frame size
    {
        // heavily inspired by atrac3tool and completed through
        // reverse engineering of ffmpeg output ...
        uint32_t i = 0xC + 8 + 0x20 + 8 + dataSz - 8;

        waveFile.write("RIFF", 4);
        putNum(i, waveFile, 4);
        waveFile.write("WAVEfmt ", 8);
        putNum(0x20, waveFile, 4);
        putNum(WAVE_FORMAT_SONY_SCX, waveFile, 2);                      ///< format tag
        putNum(2, waveFile, 2);                                         ///< channels
        putNum(44100, waveFile, 4);                                     ///< sample rate
        putNum(cmd == XEncCmd::LP2_ENCODE ? 16537 : 8268, waveFile, 4); ///< bytes per sec
        putNum(cmd == XEncCmd::LP2_ENCODE ? 0x180 : 0xc0, waveFile, 4); ///< block align
        putNum(0, waveFile, 2);                                         ///< bits per sample
        putNum(0xE, waveFile, 2);                                       ///< cb size
        if (cmd == XEncCmd::LP2_ENCODE)
        {
            waveFile.write("\x01\x00\x44\xAC\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00", 0xE);
        }
        else if (cmd == XEncCmd::LP4_ENCODE)
        {
            waveFile.write("\x01\x00\x44\xAC\x00\x00\x01\x00\x01\x00\x01\x00\x00\x00", 0xE);
        }
        waveFile.write("data", 4);
        putNum(dataSz, waveFile, 4);
        ret = 0;
    }
    return ret;
}

void CXEnc::finishCopy(int exitCode, ExitStatus exitStatus)
{
    if ((exitCode == 0) && (exitStatus == ExitStatus::NormalExit))
    {
        // open atrac file for size check
        QFile fAtrac(mAtracFileName, this);

        if (fAtrac.open(QIODevice::ReadOnly))
        {
            // get file size
            QFileInfo atracInfo(fAtrac);
            size_t sz = atracInfo.size() - ATRAC3_HEADER_SIZE;

            // overwrite original wave file
            QFile waveFile(mAtracFileName.mid(0, mAtracFileName.lastIndexOf(QChar('.'))));

            if (waveFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
            {
                // create wave header
                atrac3WaveHeader(waveFile, mCurrCmd, sz);

                char buff[ATRAC3_HEADER_SIZE];
                size_t read = 0;

                // drop atrac 3 header
                do { read += fAtrac.read(buff + read, ATRAC3_HEADER_SIZE - read); } while(read < ATRAC3_HEADER_SIZE);
                waveFile.write(fAtrac.readAll());
                waveFile.close();
            }

            fAtrac.close();
            fAtrac.remove();
        }
    }
    emit fileDone(false);
}
