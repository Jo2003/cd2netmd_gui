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
#include <QFile>
#include "defines.h"

class CXEnc : public CCliProcess
{
    Q_OBJECT
    static constexpr const char* XENC_CLI = "toolchain/atracdenc.exe";

    /// Sony WAVE format
    static constexpr uint32_t WAVE_FORMAT_SONY_SCX = 624;

    /// atrac3 header size in bytes
    static constexpr uint32_t ATRAC3_HEADER_SIZE = 96;

    /// block alignment for LP2 (used for DAO mode)
    static constexpr uint32_t ATRAC3_LP2_BLOCK_ALIGN = 384;

    /// block alignment for LP4
    static constexpr uint32_t ATRAC3_LP4_BLOCK_ALIGN = 192;

public:
    enum class XEncCmd : uint8_t
    {
        NONE,
        LP2_ENCODE,
        LP4_ENCODE,
        DAO_LP2_ENCODE
    };

    explicit CXEnc(QObject *parent = nullptr);

    int start(XEncCmd cmd, const QString& tmpFileName, uint32_t trackLength);
    int start(XEncCmd cmd, const c2n::TransferQueue& queue, uint32_t discLength);

protected:
    int atrac3WaveHeader(QFile& waveFile, XEncCmd cmd, size_t dataSz, int length);
    int splitAtrac3();

private slots:
    void finishCopy(int exitCode, ExitStatus exitStatus);

signals:
    void fileDone(bool);

protected:
    XEncCmd mCurrCmd;
    QString mAtracFileName;
    c2n::TransferQueue mQueue;
    uint32_t mLength;
};
