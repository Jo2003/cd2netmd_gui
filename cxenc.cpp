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
#include "helpers.h"
#include <cmath>

CXEnc::CXEnc(QObject *parent)
    : CCliProcess(parent), mCurrCmd(XEncCmd::NONE), mLength(0)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CXEnc::finishCopy);
}

int CXEnc::start(XEncCmd cmd, const QString& tmpFileName, uint32_t trackLength)
{
    QStringList params;
    mLog.clear();
    mCurrCmd       = cmd;
    mLength        = trackLength;
    mAtracFileName = tmpFileName + ".aea";

    switch (cmd)
    {
    case XEncCmd::DAO_LP2_ENCODE:
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

int CXEnc::start(CXEnc::XEncCmd cmd, const c2n::TransferQueue &queue, uint32_t discLength)
{
    mQueue  = queue;
    return start(cmd, queue.at(0).mpFile->fileName(), discLength);
}

int CXEnc::atrac3WaveHeader(QFile& waveFile, XEncCmd cmd, size_t dataSz, int length)
{
    int ret = -1;

    if ((waveFile.isOpen())
        && ((cmd == XEncCmd::LP2_ENCODE) || (cmd == XEncCmd::LP4_ENCODE) || (cmd == XEncCmd::DAO_LP2_ENCODE))
        && (dataSz > 192)) // 1x lp4 frame size
    {
        // heavily inspired by atrac3tool and completed through
        // reverse engineering of ffmpeg output ...
        uint32_t i = 0xC + 8 + 0x20 + 8 + dataSz - 8;

        waveFile.write("RIFF", 4);
        putNum(i, waveFile, 4);
        waveFile.write("WAVEfmt ", 8);
        putNum(0x20, waveFile, 4);
        putNum(WAVE_FORMAT_SONY_SCX, waveFile, 2);                         ///< format tag
        putNum(2, waveFile, 2);                                            ///< channels
        putNum(44100, waveFile, 4);                                        ///< sample rate
        putNum(static_cast<uint32_t>(ceil(dataSz / length)), waveFile, 4); ///< bytes per sec
        if ((cmd == XEncCmd::LP2_ENCODE) || (cmd == XEncCmd::DAO_LP2_ENCODE))
        {
            putNum(ATRAC3_LP2_BLOCK_ALIGN, waveFile, 4);                   ///< block align
        }
        else if (cmd == XEncCmd::LP4_ENCODE)
        {
            putNum(ATRAC3_LP4_BLOCK_ALIGN, waveFile, 4);                   ///< block align
        }
        putNum(0, waveFile, 2);                                            ///< bits per sample
        putNum(0xE, waveFile, 2);                                          ///< cb size
        if ((cmd == XEncCmd::LP2_ENCODE) || (cmd == XEncCmd::DAO_LP2_ENCODE))
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

int CXEnc::splitAtrac3()
{
    // open atrac file for size check
    QFile fAtrac(mAtracFileName, this);

    if (fAtrac.open(QIODevice::ReadOnly))
    {
        // get file size
        QFileInfo atracInfo(fAtrac);
        size_t sz = atracInfo.size() - ATRAC3_HEADER_SIZE;

        // drop atrac 3 header
        char buff[ATRAC3_HEADER_SIZE];
        size_t read = 0;
        do { read += fAtrac.read(buff + read, ATRAC3_HEADER_SIZE - read); } while(read < ATRAC3_HEADER_SIZE);

        // average blocks per second
        float blocksPerSec = static_cast<float>(sz / ATRAC3_LP2_BLOCK_ALIGN) / static_cast<float>(mLength);

        int trkCount = 0;

        for (const auto& t : mQueue)
        {
            trkCount ++;

            // overwrite original wave file
            QFile waveFile(t.mpFile->fileName());

            if (waveFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
            {
                uint32_t copyBlocks = static_cast<uint32_t>(ceil(t.mLength * blocksPerSec));
                size_t sz = copyBlocks * ATRAC3_LP2_BLOCK_ALIGN;

                // create wave header
                atrac3WaveHeader(waveFile, mCurrCmd, sz, t.mLength);

                // copy atrac data
                if (trkCount < mQueue.size())
                {
                    waveFile.write(fAtrac.read(sz));
                }
                else
                {
                    // last track -> read all
                    waveFile.write(fAtrac.readAll());
                }
                waveFile.close();
            }
        }

        fAtrac.close();
        fAtrac.remove();
    }
    return 0;
}

void CXEnc::finishCopy(int exitCode, ExitStatus exitStatus)
{
    if ((exitCode == 0) && (exitStatus == ExitStatus::NormalExit))
    {
        if ((mCurrCmd == XEncCmd::LP2_ENCODE) || (mCurrCmd == XEncCmd::LP4_ENCODE))
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
                    atrac3WaveHeader(waveFile, mCurrCmd, sz, mLength);

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
        else if (mCurrCmd == XEncCmd::DAO_LP2_ENCODE)
        {
            splitAtrac3();
        }
    }

    if (!mLog.isEmpty())
    {
        qDebug() << mLog;
    }

    emit fileDone(false);
}
