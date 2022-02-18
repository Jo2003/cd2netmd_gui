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
#include "audio.h"
#include <QDir>
#include <cstring>
#include <stdexcept>
#include <QtEndian>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      namespace for audio handling
//------------------------------------------------------------------------------
namespace audio {

//--------------------------------------------------------------------------
//! @brief      extract range from wave file
//!
//! @param[in]  srcName  source file name
//! @param[in]  trgName  target file name
//! @param[in]  start  start block
//! @param[in]  length block count
//--------------------------------------------------------------------------
int extractRange(const QString& srcName, const QString& trgName, long start, long length)
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

//--------------------------------------------------------------------------
//! @brief      forward file position to wave data
//!
//! @param      fWave         The wave file
//! @param      waveDataSize  The raw wave data size
//!
//! @return     0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int stripWaveHeader(QFile& fWave, size_t& waveDataSize)
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

//------------------------------------------------------------------------------
//! @brief      Writes a wave header.
//!
//! @param      wf         wave file
//! @param[in]  byteCount  The byte count
//!
//! @return     0
//------------------------------------------------------------------------------
int writeWaveHeader(QFile &wf, size_t byteCount)
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

//--------------------------------------------------------------------------
//! @brief      check audio file for conversion needs
//!
//! @param      fileName      The audio file name
//! @param      conversion    The conversion vector @see AudioConv
//! @param      length        length in mili seconds
//! @param      pTag[in]      optional pointer to tag structure
//!
//! @return     0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int checkAudioFile(const QString& fileName, uint32_t& conversion, int& length, STag* pTag)
{
    int ret    = 0;
    conversion = 0;

    QFileInfo fi(fileName);
    QString ext = fi.suffix().toLower();
    TagLib::FileRef f(static_cast<const char*>(fileName.toUtf8()));

    if (!f.isNull())
    {
        if (ext == "wav")
        {
            AudioProps<WAVE>::type props = reinterpret_cast<AudioProps<WAVE>::type>(f.audioProperties());
            if (props->bitsPerSample() != 16)
            {
                conversion |= AudioConv::CONV_BPS;
            }

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else if ((ext == "m4a") || (ext == "mp4"))
        {
            AudioProps<M4A>::type props = reinterpret_cast<AudioProps<M4A>::type>(f.audioProperties());
            conversion |= AudioConv::CONV_FORMAT;

            if (props->bitsPerSample() != 16)
            {
                conversion |= AudioConv::CONV_BPS;
            }

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else if (ext == "mp3")
        {
            AudioProps<MP3>::type props = reinterpret_cast<AudioProps<MP3>::type>(f.audioProperties());
            conversion |= AudioConv::CONV_FORMAT;
            conversion |= AudioConv::CONV_BPS;

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else if (ext == "ogg")
        {
            AudioProps<OGG>::type props = reinterpret_cast<AudioProps<OGG>::type>(f.audioProperties());
            conversion |= AudioConv::CONV_FORMAT;
            conversion |= AudioConv::CONV_BPS;

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else if (ext == "flac")
        {
            AudioProps<FLAC>::type props = reinterpret_cast<AudioProps<FLAC>::type>(f.audioProperties());
            conversion |= AudioConv::CONV_FORMAT;

            if (props->bitsPerSample() != 16)
            {
                conversion |= AudioConv::CONV_BPS;
            }

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else if (ext == "ape")
        {
            AudioProps<APE>::type props = reinterpret_cast<AudioProps<APE>::type>(f.audioProperties());
            conversion |= AudioConv::CONV_FORMAT;

            if (props->bitsPerSample() != 16)
            {
                conversion |= AudioConv::CONV_BPS;
            }

            if (props->sampleRate() != 44100)
            {
                conversion |= AudioConv::CONV_SAMPLERATE;
            }

            if (props->channels() > 2)
            {
                conversion |= AudioConv::CONV_CHANNELS;
            }

            length = props->lengthInMilliseconds();
        }
        else
        {
            qWarning() << "Unsupported audio file extension:" << ext << "on file" << fi.fileName();
            ret = -1;
        }

        if ((pTag != nullptr) && (ret == 0))
        {
            pTag->mAlbum  = QString::fromUtf8(f.tag()->album().toCString());
            pTag->mArtist = QString::fromUtf8(f.tag()->artist().toCString());
            pTag->mTitle  = QString::fromUtf8(f.tag()->title().toCString());
            pTag->mNumber = f.tag()->track();
            pTag->mYear   = f.tag()->year();
        }
    }
    else
    {
        qWarning() << "Tag lib can't parse" << fi.absoluteFilePath();
        ret = -1;
    }
    return ret;
}

}
