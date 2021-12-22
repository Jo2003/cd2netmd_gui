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
#include <QVector>
#include <QTemporaryFile>
#include <cstdint>

namespace c2n {

enum class WorkStep : uint8_t
{
    NONE,       // steady
    RIP,        // transient
    RIPPED,     // steady
    ENCODE,     // transient
    ENCODED,    // steady
    TRANSFER,   // transient
    DONE        // steady
};

struct SRipTrack
{
    int16_t         mCDTrackNo;
    QString         mTitle;
    QTemporaryFile* mpFile;
    time_t          mLength;
    WorkStep        mStep;
};

using TransferQueue = QVector<SRipTrack>;

static constexpr const char* PROGRAM_VERSION = "1.5.3";
static constexpr const char* PROGRAM_NAME    = "CD2NetMD GUI";

}
