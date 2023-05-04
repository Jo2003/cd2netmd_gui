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
//! @param[in]  pTag          optional pointer to tag structure
//! @param[in]  sp_upload     optional sp upload flag
//!
//! @return     0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int checkAudioFile(const QString& fileName, uint32_t& conversion, int& length, STag* pTag, bool sp_upload)
{
    int ret    = 0;
    conversion = 0;

    QFileInfo fi(fileName);
    QString ext = fi.suffix().toLower();
#ifdef Q_OS_WIN
    TagLib::FileRef f(reinterpret_cast<const wchar_t *>(fileName.utf16()));
#else
    TagLib::FileRef f(static_cast<const char*>(fileName.toUtf8()));
#endif
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
            pTag->mAlbum  = QString::fromStdString(f.tag()->album().to8Bit(true));
            pTag->mArtist = QString::fromStdString(f.tag()->artist().to8Bit(true));
            pTag->mTitle  = QString::fromStdString(f.tag()->title().to8Bit(true));
            pTag->mNumber = f.tag()->track();
            pTag->mYear   = f.tag()->year();
        }
    }
    else if (ext == "aea")
    {
        // we might have found an Atrac 1 (SP) file -
        // do some basic checks
        try
        {
            if (fi.size() < (2048 + 212))
            {
                throw "File size to small for Atrac 1";
            }

            QFile atrac(fileName);
            if (!atrac.open(QIODevice::ReadOnly))
            {
                throw "Can't open Atrac 1 file!";
            }

            char adata[2048] = {'\0',};
            if (atrac.read(adata, 2048) != 2048)
            {
                throw "Can't read Atrac 1 header!";
            }

            // magic is 0x00, 0x08, 0x00, 0x00
            constexpr uint8_t magic[] = {0x00, 0x08, 0x00, 0x00};
            if (memcmp(adata, magic, 4))
            {
                throw "Atrac 1 magic header doesn't match!";
            }

            uint8_t channels = static_cast<uint8_t>(adata[264]);

            if ((channels != 1) && (channels != 2))
            {
                throw "Wrong Atrac 1 channel count!";
            }

            // mono or stereo -> looks like a valid Atrac1 file
            if (pTag != nullptr)
            {
                // we don't have much information to fill in ...
                pTag->mAlbum  = "";
                pTag->mArtist = "";
                pTag->mTitle  = fi.baseName();
                pTag->mNumber = 1;
                pTag->mYear   = 1992;
            }

            // the bitrate for atrac 1 is 292kbit/s, mind the header
            length = qRound((static_cast<double>(fi.size()) - 2048.0) / 292000.0 * 8.0 * 1000.0);

            // mono uses half bitrate ...
            if (channels == 1)
            {
                length *= 2;
            }
            ret = 0;

            if (!sp_upload)
            {
                // if no SP upload is supported, we
                // convert to Wave
                conversion |= AudioConv::CONV_BPS;
                conversion |= AudioConv::CONV_SAMPLERATE;
                conversion |= AudioConv::CONV_CHANNELS;
            }
        }
        catch (const char* e)
        {
            qInfo() << e;
            ret = -1;
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
