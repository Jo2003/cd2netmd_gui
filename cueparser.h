/**
 * Copyright (C) 2023 Jo2003 (olenka.joerg@gmail.com)
 * This file is part of cd2netmd_gui (NetMD Wizard)
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NetMD Wizard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#pragma once
#include "audio.h"
#include <QString>
#include <QTextStream>
#include <QVector>

//------------------------------------------------------------------------------
//! @brief      This class describes a simple cue parser.
//------------------------------------------------------------------------------
class CueParser
{
public:

    //--------------------------------------------------------------------------
    //! @brief      track type
    //--------------------------------------------------------------------------
    enum TrackType
    {
        AUDIO,  ///< Audio
        DATA    ///< Data
    };

    //--------------------------------------------------------------------------
    //! @brief      describes an audio track
    //--------------------------------------------------------------------------
    struct Track
    {
        int mNo;            ///< track number (not index based)
        TrackType mType;    ///< type \a TrackType
        QString mTitle;     ///< track title
        QString mPerformer; ///< artist / performer
        QString mAudioFile; ///< audio file name
        uint32_t mStartMs;  ///< start position in ms related to audio file
        uint32_t mEndMs;    ///< end position related to audio file

        //----------------------------------------------------------------------
        //! @brief      Qstring conversion operator.
        //----------------------------------------------------------------------
        operator QString() const
        {
            QString s;
            QTextStream ts(&s);
            ts << mNo << ") " << (!mPerformer.isEmpty() ? (mPerformer + " - ") : "") << mTitle << Qt::endl
               << "\tFile: " << mAudioFile << Qt::endl
               << "\tPosition: " << mStartMs << " ms ... " << mEndMs
               << " ms, type: " << ((mType == AUDIO) ? "AUDIO" : "DATA");
            return s;
        }
    };

    //--------------------------------------------------------------------------
    //! @brief      describes an audio disc (CD)
    //--------------------------------------------------------------------------
    struct Disc
    {
        QString mTitle;             ///< disc title
        QString mPerformer;         ///< disc performer 7 artist
        uint32_t mLenInMs;          ///< disc length in ms
        QVector<Track> mTracks;     ///< audio tracks

        //----------------------------------------------------------------------
        //! @brief      Qstring conversion operator.
        //----------------------------------------------------------------------
        operator QString() const
        {
            QString s;
            QTextStream ts(&s);
            ts << (!mPerformer.isEmpty() ? (mPerformer + " - ") : "") << mTitle << " (" << mLenInMs << " ms)\n";
            for (const auto& t : mTracks)
            {
                ts << static_cast<QString>(t) << Qt::endl;
            }
            return s;
        }
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  cueFileName  The cue file name
    //--------------------------------------------------------------------------
    CueParser(const QString& cueFileName = "");

    //--------------------------------------------------------------------------
    //! @brief      parse the cue file
    //!
    //! @param[in]  cueFileName  The cue file name
    //!
    //! @return     0 on success; < 0 on error
    //--------------------------------------------------------------------------
    int parse(const QString& cueFileName);

    //--------------------------------------------------------------------------
    //! @brief      get track count
    //!
    //! @return     number of tracks
    //--------------------------------------------------------------------------
    int trackCount() const;

    //--------------------------------------------------------------------------
    //! @brief      git disc title
    //!
    //! @return     title as string
    //--------------------------------------------------------------------------
    const QString& discTitle() const;

    //--------------------------------------------------------------------------
    //! @brief      get disc artist / performer
    //!
    //! @return     artist as string
    //--------------------------------------------------------------------------
    const QString& discPerfromer() const;

    //--------------------------------------------------------------------------
    //! @brief      get disc audio length in ms
    //!
    //! @return     length in ms
    //--------------------------------------------------------------------------
    uint32_t discLength() const;

    //--------------------------------------------------------------------------
    //! @brief      get complete track information
    //!
    //! @param[in]  no    track number, starts with 1
    //!
    //! @return     track information or empty struct
    //--------------------------------------------------------------------------
    const Track& track(int no);

private:
    /// stores disc information
    Disc  mDiscData{"", "", 0, {}};

    /// stores an empty track
    Track mEmptyTrack{-1, DATA, "", "", "", 0, 0};
};
