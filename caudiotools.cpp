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
#include "caudiotools.h"
#include <QDir>
#include <cstring>
#include <stdexcept>
#include <QtEndian>
#include "defines.h"

//--------------------------------------------------------------------------
//! @brief      extract range from wave file
//!
//! @param[in]  srcName  source file name
//! @param[in]  trgName  target file name
//! @param[in]  start  start block
//! @param[in]  length block count
//--------------------------------------------------------------------------
int CAudioTools::extractRange(const QString& srcName, const QString& trgName, long start, long length)
{
    int ret = 0;
    QFile sourceWave(srcName);
    if (sourceWave.open(QIODevice::ReadOnly))
    {
        /// Note! We have to cut at frame border (% 2048)
        size_t wholeSz = 0, read = 0;
        if (stripWaveHeader(sourceWave, wholeSz) == 0)
        {
            if (start != 0)
            {
                uint64_t pos = start * RAW_BLOCK_SIZE;
                pos -= (pos % WAVE_FRAME_SIZE);
                wholeSz -= pos;
                sourceWave.seek(sourceWave.pos() + pos);
            }

            read = length * RAW_BLOCK_SIZE;
            read -= (read % WAVE_FRAME_SIZE);

            // if difference between read vs. wholeSz is in range of 2 seconds, we read complete file.
            if (std::abs(static_cast<long>(wholeSz - read)) < static_cast<long>(WAVE_BLOCK_SIZE * 2))
            {
                read = wholeSz;
            }

            QFile targetFile(trgName);
            if (targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                writeWaveHeader(targetFile, read);
                targetFile.write(sourceWave.read(read));
            }
            else
            {
                qWarning() << "Can't open target file:" << trgName;
                ret = -1;
            }
        }
        else
        {
            qWarning() << "Can't find wave data in:" << srcName;
            ret = -1;
        }
    }
    else
    {
        qWarning() << "Can't open source file:" << srcName;
        ret = -1;
    }

    return ret;
}

//------------------------------------------------------------------------------
//! @brief      check if wave file is valid
//!
//! @param      fWave         The wave file
//! @param      waveDataSize  The wave data size
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CAudioTools::checkWaveFile(QFile &fWave, size_t &waveDataSize)
{
    int ret = 0;
    fWave.seek(0);
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
        qInfo() << e.what();
        ret = -1;
    }
    return ret;
}

//--------------------------------------------------------------------------
//! @brief      forward file position to wave data
//!
//! @param      fWave         The wave file
//! @param      waveDataSize  The raw wave data size
//!
//! @return     0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CAudioTools::stripWaveHeader(QFile& fWave, size_t& waveDataSize)
{
    int ret = 0;
    fWave.seek(0);
    try
    {
        // read WAV header
        QByteArray data = fWave.read(256);
        int pos;

        if ((pos = data.indexOf("data")) == -1)
        {
            throw std::runtime_error(R"("data" marker not detected!)");
        }
        else
        {
            waveDataSize = qFromLittleEndian<uint32_t>(static_cast<const char*>(data) + pos + 4);
            fWave.seek(pos + 8);
        }
    }
    catch (const std::exception& e)
    {
        qInfo() << e.what();
        ret = -1;
    }
    return ret;
}

//--------------------------------------------------------------------------
//! @brief      check audio file for fLaC marker
//!
//! @param[in]  fAudio   The audio file to check
//! @param[out] duration audio length
//!
//! @return     true if flac, false if not
//--------------------------------------------------------------------------
bool CAudioTools::isFlac(QFile& fAudio, int& duration)
{
    bool ret = false;
    duration = 0;
    fAudio.seek(0);

    QByteArray data = fAudio.read(8);
    ret = (data.indexOf("fLaC") == 0);

    // check for stream info meta block
    if ((data[4] >> 1) == 0)
    {
        // get meta block size
        int metaSz = arrayToUint(static_cast<const char*>(data) + 5, 3);

        // read stream info block
        data = fAudio.read(metaSz);

        qDebug() << "Meta Size on" << fAudio.fileName() << "is" << metaSz;

        uint64_t sampleRate = arrayToUint(static_cast<const char*>(data) + 10, 3) >> 4;
        char cSamples[5] = {0,};
        memcpy(cSamples, static_cast<const char*>(data) + 13, 5);
        cSamples[0] = cSamples[0] & 0x0f;
        uint64_t samples = arrayToUint(cSamples, 5);

        duration = samples / sampleRate;

        if (sampleRate != 44100)
        {
            qWarning() << "Only 44.1kHz sampling rate supported!";
            ret = false;
        }

        qDebug() << "Samples:" << samples << "Rate:" << sampleRate << "Duration:" << duration;
    }

    return ret;
}

//------------------------------------------------------------------------------
//! @brief      Writes a wave header.
//!
//! @param      wf         wave file
//! @param[in]  byteCount  The byte count
//!
//! @return     0
//------------------------------------------------------------------------------
int CAudioTools::writeWaveHeader(QFile &wf, size_t byteCount)
{
    wf.write("RIFF", 4);                // 0
    putNum(byteCount + 44 - 8, wf, 4);  // 4
    wf.write("WAVEfmt ", 8);            // 8
    putNum(16, wf, 4);                  // 16
    putNum(1, wf, 2);                   // 20
    putNum(2, wf, 2);                   // 22
    putNum(44100, wf, 4);               // 24
    putNum(44100 * 2 * 2, wf, 4);       // 28
    putNum(4, wf, 2);                   // 32
    putNum(16, wf, 2);                  // 34
    wf.write("data", 4);                // 36
    putNum(byteCount, wf, 4);           // 40

    return 0;
}

