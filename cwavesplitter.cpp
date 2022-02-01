/**
 * Copyright (C) 2022 Jo2003 (olenka.joerg@gmail.com)
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
#include "cwavesplitter.h"
#include <QDir>
#include <cstring>
#include <stdexcept>
#include <QtEndian>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent  The parent
//------------------------------------------------------------------------------
CWaveSplitter::CWaveSplitter(QObject *parent)
    :QObject(parent)
{
}

//------------------------------------------------------------------------------
//! @brief      Splits waves.
//!
//! @param[in]  queue  The job queue
//------------------------------------------------------------------------------
void CWaveSplitter::splitWaves(const c2n::TransferQueue &queue)
{
    QFile sourceWave(queue[0].mpFile->fileName());
    if (sourceWave.open(QIODevice::ReadOnly))
    {
        size_t wholeSz = 0, read = 0;
        if (checkWaveFile(sourceWave, wholeSz) == 0)
        {
            int trackCount = 0;
            for (const auto& t : queue)
            {
                QString fileName = t.mpFile->fileName();

                trackCount ++;

                if (trackCount == 1)
                {
                    fileName += "buf";
                }

                QFile track(fileName);
                if (track.open(QIODevice::Truncate | QIODevice::WriteOnly))
                {
                    size_t sz = t.mLength * WAVE_BLOCK_SIZE;
                    if (sz % WAVE_FRAME_SIZE)
                    {
                        sz += WAVE_FRAME_SIZE - (sz % WAVE_FRAME_SIZE);
                    }

                    if (trackCount == queue.size())
                    {
                        writeWaveHeader(track, wholeSz - read);
                        track.write(sourceWave.readAll());
                        qDebug("Wrote last file %s with %llu bytes",
                               static_cast<const char*>(track.fileName().toUtf8()), wholeSz - read);
                    }
                    else
                    {
                        writeWaveHeader(track, sz);
                        track.write(sourceWave.read(sz));
                        read += sz;
                        qDebug("Wrote file %s with %llu bytes",
                               static_cast<const char*>(track.fileName().toUtf8()), sz);
                    }
                    track.close();
                }
            }

            sourceWave.close();

            // copy first track
            QFile f1(queue.at(0).mpFile->fileName());
            QFile f2(queue.at(0).mpFile->fileName() + "buf");

            if (f1.open(QIODevice::Truncate | QIODevice::WriteOnly) && f2.open(QIODevice::ReadOnly))
            {
                f1.write(f2.readAll());
                f1.close();
                f2.close();
                f2.remove();
            }
        }
    }
}

//------------------------------------------------------------------------------
//! @brief      check if wave file is valid
//!
//! @param      fWave         The wave file
//! @param      waveDataSize  The wave data size
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CWaveSplitter::checkWaveFile(QFile &fWave, size_t &waveDataSize)
{
    int ret = 0;
    try
    {
        // read WAV header as written by our Jack the Ripper
        QByteArray data = fWave.read(256);
        int pos;

        if (data.indexOf("RIFF") == -1)
        {
            throw std::runtime_error(R"("RIFF" marker not detected!)");
        }
        else if (data.indexOf("WAVEfmt ") == -1)
        {
            throw std::runtime_error(R"("WAVEfmt " marker not detected!)");
        }
        else if ((pos = data.indexOf("data")) == -1)
        {
            throw std::runtime_error(R"("data" marker not detected!)");
        }
        else
        {
            waveDataSize = qFromLittleEndian<uint32_t>(static_cast<const char*>(data) + pos + 4);
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << e.what();
        ret = -1;
    }
    return ret;
}
