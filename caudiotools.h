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
#include <QObject>
#include "helpers.h"
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      This class describes a wave splitter.
//------------------------------------------------------------------------------
class CAudioTools : public QObject
{
    Q_OBJECT

    // data needed for 1 sec in 44100KHz, 16bit, stereo
    static constexpr uint32_t WAVE_BLOCK_SIZE = 44100 * 2 * 2;
    static constexpr uint32_t WAVE_FRAME_SIZE = 2048;
    static constexpr uint32_t RAW_BLOCK_SIZE = 2352;

public:

    //--------------------------------------------------------------------------
    //! @brief      extract range from wave file
    //!
    //! @param[in]  srcName  source file name
    //! @param[in]  trgName  target file name
    //! @param[in]  start  start block
    //! @param[in]  length block count
    //--------------------------------------------------------------------------
    static int extractRange(const QString& srcName, const QString& trgName, long start, long length);

    //------------------------------------------------------------------------------
    //! @brief      Writes a wave header.
    //!
    //! @param      wf         wave file
    //! @param[in]  byteCount  The byte count
    //!
    //! @return     0
    //------------------------------------------------------------------------------
    static int writeWaveHeader(QFile &wf, size_t byteCount);
    
    //--------------------------------------------------------------------------
    //! @brief      check wave file
    //!
    //! @param      fWave         The wave file
    //! @param      waveDataSize  The wave data size
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    static int checkWaveFile(QFile& fWave, size_t& waveDataSize);

    //--------------------------------------------------------------------------
    //! @brief      check audio file for fLaC marker
    //!
    //! @param[in]  fAudio   The audio file to check
    //! @param[out] duration audio length
    //!
    //! @return     true if flac, false if not
    //--------------------------------------------------------------------------
    static bool isFlac(QFile& fAudio, int& duration);

    //--------------------------------------------------------------------------
    //! @brief      forward file position to wave data
    //!
    //! @param      fWave         The wave file
    //! @param      waveDataSize  The raw wave data size
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    static int stripWaveHeader(QFile& fWave, size_t& waveDataSize);

};
