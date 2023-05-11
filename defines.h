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
#include <QtDebug>

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
    double          mLength;
    WorkStep        mStep;
    bool            mIsCD;
};

using TransferQueue = QVector<SRipTrack>;

enum TrackType
{
    AUDIO = 0,
    DISC  = 1,
    DATA  = 3,
};

struct STrackInfo
{
    STrackInfo(const QString& title = "",
               const QString& fName = "",
               const QString& wName = "",
               int cdt = -1, long lba = 0,
               long lenb = -1, uint32_t conv = 0,
               TrackType tt = TrackType::AUDIO)
        : mTitle(title), mFileName(fName),
          mWaveFileName(wName), mCDTrackNo(cdt),
          mStartLba(lba), mLbCount(lenb),
          mConversion(conv),
          mTType(tt)
    {}

    QString   mTitle;
    QString   mFileName;
    QString   mWaveFileName;
    int       mCDTrackNo;
    long      mStartLba;
    long      mLbCount;
    uint32_t  mConversion;
    TrackType mTType;
};

///
/// \brief The AudioTracks class
///
class AudioTracks : public QVector<STrackInfo>
{
public:
    ///
    /// \brief What may be stored in list
    ///
    enum List_t
    {
        CD,         ///< CD tracks
        FILES,      ///< files
        CUE_SHEET,  ///< cue sheet
        UNKNOWN     ///< unknown
    };

    ///
    /// Using base class constructor
    ///
    using QVector::QVector;

    ///
    /// \brief set the List Type
    /// \param t type
    ///
    void setListType(List_t t)
    {
        mType = t;
    }

    ///
    /// \brief get the list Type
    /// \return type
    ///
    List_t listType() const
    {
        return mType;
    }

private:
    /// list type
    List_t mType = List_t::UNKNOWN;
};

static constexpr const char* PROGRAM_VERSION = "2.0.4";
static constexpr const char* PROGRAM_NAME    = "NetMD Wizard";

enum LogLevel
{
    DEBUG,
    INFO,
    WARN,
    CRITICAL,
    FATAL
};

}

extern const QString g_logFileName;
extern c2n::LogLevel g_LogFilter;

namespace styles {
constexpr const char* CD_TAB_STD = R"(#tableViewCD {
    background-repeat: no-repeat;
    background-position: center fixed;
    background-image: url(:/view/dnd);
    background-attachment: fixed;
    background-color: #fff;
})";

constexpr const char* CD_TAB_STYLED = R"(#tableViewCD {
    background-repeat: no-repeat;
    background-position: center fixed;
    background-image: url(:/view/dnd);
    background-attachment: fixed;
})";
}
