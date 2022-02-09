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
#pragma once
#include "helpers.h"
#include "defines.h"
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/audioproperties.h>
#include <taglib/apeproperties.h>
#include <taglib/flacproperties.h>
#include <taglib/mp4properties.h>
#include <taglib/mpegproperties.h>
#include <taglib/vorbisproperties.h>
#include <taglib/wavproperties.h>

//------------------------------------------------------------------------------
//! @brief      namespace for audio handling
//------------------------------------------------------------------------------
namespace audio {

    // data needed for 1 sec in 44100KHz, 16bit, stereo
    constexpr uint32_t WAVE_BLOCK_SIZE = 44100 * 2 * 2;
    constexpr uint32_t WAVE_FRAME_SIZE = 2048;
    constexpr uint32_t RAW_BLOCK_SIZE  = 2352;

    /// needed audio conversion
    enum AudioConv {
        CONV_FORMAT     = (1ul << 0),   ///< convert to wave
        CONV_BPS        = (1ul << 1),   ///< convert to 16bit
        CONV_SAMPLERATE = (1ul << 2),   ///< convert to 44.1kHz
        CONV_CHANNELS   = (1ul << 3),   ///< convert multichannel to stereo
    };

    enum Supported {
        WAVE,
        FLAC,
        OGG,
        APE,
        M4A,
        MP3
    };

    //--------------------------------------------------------------------------
    //! @brief      extract range from wave file
    //!
    //! @param[in]  srcName  source file name
    //! @param[in]  trgName  target file name
    //! @param[in]  start  start block
    //! @param[in]  length block count
    //--------------------------------------------------------------------------
    int extractRange(const QString& srcName, const QString& trgName, long start, long length);

    //------------------------------------------------------------------------------
    //! @brief      Writes a wave header.
    //!
    //! @param      wf         wave file
    //! @param[in]  byteCount  The byte count
    //!
    //! @return     0
    //------------------------------------------------------------------------------
    int writeWaveHeader(QFile &wf, size_t byteCount);
    
    //--------------------------------------------------------------------------
    //! @brief      check wave file
    //!
    //! @param      fWave         The wave file
    //! @param      waveDataSize  The wave data size
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int checkWaveFile(QFile& fWave, size_t& waveDataSize);

    //--------------------------------------------------------------------------
    //! @brief      check audio file for fLaC marker
    //!
    //! @param[in]  fAudio   The audio file to check
    //! @param[out] duration audio length
    //!
    //! @return     true if flac, false if not
    //--------------------------------------------------------------------------
    bool isFlac(QFile& fAudio, int& duration);

    //--------------------------------------------------------------------------
    //! @brief      forward file position to wave data
    //!
    //! @param      fWave         The wave file
    //! @param      waveDataSize  The raw wave data size
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int stripWaveHeader(QFile& fWave, size_t& waveDataSize);

    //--------------------------------------------------------------------------
    //! @brief      check audio file for conversion needs
    //!
    //! @param      fileName      The audio file name
    //! @param      conversion    The conversion vector @see AudioConv
    //! @param      length        length in mili seconds
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int checkAudioFile(const QString& fileName, uint32_t& conversion, int& length);

    template <Supported T> struct AudioProps;
    template <> struct AudioProps<APE>  { using type = TagLib::APE::Properties*;       }; // has bitsPerSample()
    template <> struct AudioProps<FLAC> { using type = TagLib::FLAC::Properties*;      }; // has bitsPerSample()
    template <> struct AudioProps<M4A>  { using type = TagLib::MP4::Properties*;       }; // has bitsPerSample()
    template <> struct AudioProps<MP3>  { using type = TagLib::MPEG::Properties*;      }; // hasn't bitsPerSample()
    template <> struct AudioProps<OGG>  { using type = TagLib::Vorbis::Properties*;    }; // hasn't bitsPerSample()
    template <> struct AudioProps<WAVE> { using type = TagLib::RIFF::WAV::Properties*; }; // has bitsPerSample()
}
