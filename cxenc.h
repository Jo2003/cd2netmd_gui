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
#pragma once
#include "ccliprocess.h"

class CXEnc : public CCliProcess
{
    Q_OBJECT
    static constexpr const char* XENC_CLI = "toolchain/atracdenc.exe";

    /// Sony WAVE format
    static constexpr uint32_t WAVE_FORMAT_SONY_SCX = 624;

    struct WaveFormatEx
    {
        uint16_t wFormatTag;
        uint16_t nChannels;
        uint32_t nSamplesPerSec;
        uint32_t nAvgBytesPerSec;
        uint16_t nBlockAlign;
        uint16_t wBitsPerSample;
        uint16_t cbSize;
    };

public:
    enum class XEncCmd : uint8_t
    {
        NONE,
        LP2_ENCODE,
        LP4_ENCODE
    };

    explicit CXEnc(QObject *parent = nullptr);

    int start(XEncCmd cmd, const QString& tmpFileName);

protected:
    int atrac3WaveHeader(const QString& tmpFileName, XEncCmd cmd, uint32_t dataSz);

private slots:

protected:
    XEncCmd   mCurrCmd;
};
