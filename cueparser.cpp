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
#include "cueparser.h"
#include <QFile>
#include <QFileInfo>
#include <QRegExp>

///
/// \brief The CueThrow struct
///
struct CueThrow
{
    int mErr;
    QString mSErr;
};


/// \brief helper marco to throw an error
#define mCueThrow(x_, ...) { QString se_; QTextStream tse_(&se_); tse_ << __VA_ARGS__; throw CueThrow{x_, se_};}

//--------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param[in]  cueFileName  The cue file name
//--------------------------------------------------------------------------
CueParser::CueParser(const QString &cueFileName)
{
    if (!cueFileName.isEmpty())
    {
        parse(cueFileName);
    }
}

//--------------------------------------------------------------------------
//! @brief      parse the cue file
//!
//! @param[in]  cueFileName  The cue file name
//!
//! @return     0 on success; < 0 on error
//--------------------------------------------------------------------------
int CueParser::parse(const QString &cueFileName)
{
    int ret = 0;

    // You thought this would be nice? You're wrong!

    // get title string
    QRegExp rxpTitle("^TITLE\\s+(.*)$");

    // get performer
    QRegExp rxpPerf("^PERFORMER\\s+(.*)$");

    // get track
    QRegExp rxpTrack("^TRACK\\s+([0-9]+)\\s+([A-Za-z]+)$");

    // get INDEX 01
    QRegExp rxpIndex("^INDEX\\s+01\\s+([0-9:]+)$");

    // get audio file
    QRegExp rxpFile("^FILE\\s+(.*)\\s+([A-Za-z]+)$");

    // pre-filter
    QRegExp rxpUse("^(TITLE|PERFORMER|TRACK|INDEX\\s+01|FILE).*$");

    QFile cueFile(cueFileName);
    QFileInfo cfi(cueFileName);

    bool inTrack = false;
    Track track;
    QString file, lastFile;
    TrackType ftype = TrackType::DATA, ttype = TrackType::DATA;
    int audLengthMs = 0;
    uint32_t audConv = 0;
    mDiscData = {"", "", 0, {}};

    try
    {
        if (!cueFile.open(QIODevice::ReadOnly))
        {
            mCueThrow(-1, "Can't open cue file " << cueFileName);
        }

        QTextStream in(&cueFile);

        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();

            // check for needed information
            if (rxpUse.indexIn(line) > -1) // pre-filter!
            {
                if (rxpFile.indexIn(line) > -1) // Audio file
                {
                    lastFile = file;
                    file = rxpFile.cap(1).replace(QChar('"'), "");

                    // remove any directory part
                    QFileInfo afi(file);
                    file = afi.fileName();

                    if (lastFile != file)
                    {
                        // get additional information from audio file
                        if (audio::checkAudioFile(cfi.canonicalPath() + "/" + file, audConv, audLengthMs) != 0)
                        {
                            mCueThrow(-4, "Can't recognize audio file " << file);
                        }
                    }

                    ftype = (rxpFile.cap(2).toUpper() == "WAVE") ? TrackType::AUDIO : TrackType::DATA;
                }
                else if (rxpTitle.indexIn(line) > -1) // title (disc or track)
                {
                    if (inTrack)
                    {
                        track.mTitle = rxpTitle.cap(1).replace(QChar('"'), "");
                    }
                    else
                    {
                        mDiscData.mTitle = rxpTitle.cap(1).replace(QChar('"'), "");
                    }
                }
                else if (rxpPerf.indexIn(line) > -1) // performer / artist (disc or track)
                {
                    if (inTrack)
                    {
                        track.mPerformer = rxpPerf.cap(1).replace(QChar('"'), "");
                    }
                    else
                    {
                        mDiscData.mPerformer = rxpPerf.cap(1).replace(QChar('"'), "");
                    }
                }
                else if (rxpTrack.indexIn(line) > -1) // track
                {
                    if (inTrack)
                    {
                        // complete current track info
                        if (track.mPerformer.isEmpty())
                        {
                            track.mPerformer = mDiscData.mPerformer;
                        }

                        // store track
                        mDiscData.mTracks.append(track);

                        // cleanup track structure
                        track = {-1, TrackType::DATA, "", "", "", 0, 0, 0, 0, 0};
                    }

                    inTrack = true;
                    track.mNo = rxpTrack.cap(1).toUInt();
                    ttype = (rxpTrack.cap(2).toUpper() == "AUDIO") ? TrackType::AUDIO : TrackType::DATA;
                }
                else if (rxpIndex.indexIn(line) > -1) // index 01
                {
                    // file mentioned before INDEX 01 is the track file
                    track.mAudioFile  = file;
                    track.mConversion = audConv;
                    track.mEndMs      = static_cast<uint32_t>(audLengthMs);
                    track.mType = ((ttype == ftype) && (ftype == TrackType::AUDIO)) ? TrackType::AUDIO : TrackType::DATA;

                    QString idx = rxpIndex.cap(1);
                    auto idxs = idx.split(QChar(':'));
                    if (idxs.size() == 3)
                    {
                        uint32_t msec = idxs.at(0).toUInt() * 60 * 1000; // minutes
                        msec += idxs.at(1).toUInt() * 1000;              // seconds
                        msec += idxs.at(2).toUInt() * 10;                // 100ths of seconds
                        track.mStartMs = msec;

                        // we must mark the end of the former track
                        if (!mDiscData.mTracks.isEmpty() && msec)
                        {
                            mDiscData.mTracks[mDiscData.mTracks.size() - 1].mEndMs = msec;
                        }
                    }
                    else
                    {
                        mCueThrow(-3, "Wrong index format: " << idx);
                    }
                }
                else
                {
                    mCueThrow(-2, "Can't parse line: " << line);
                }
            }
            else
            {
                qDebug() << "Ignore line" << line;
            }
        }
        cueFile.close();

        // mind the last track
        if (inTrack)
        {
            // complete current track info
            if (track.mPerformer.isEmpty())
            {
                track.mPerformer = mDiscData.mPerformer;
            }

            // store track
            mDiscData.mTracks.append(track);
        }

        // compute disc length and LBA stuff
        uint32_t lba = 0;
        for (auto& t : mDiscData.mTracks)
        {
            t.mStartLba = lba;
            t.mLbaCount = qRound((static_cast<double>(t.mEndMs - t.mStartMs) / 1000.0) * 75.0);
            lba += t.mLbaCount;
            mDiscData.mLenInMs += t.mEndMs - t.mStartMs;
        }

        qDebug().noquote() << Qt::endl << static_cast<QString>(mDiscData);
    }
    catch (const CueThrow& e)
    {
        qWarning() << e.mSErr;
        ret = e.mErr;
    }
    catch (...)
    {
        qWarning() << "Unknown error while parsing cue file" << cueFileName;
        ret = -1;
    }

    // sanity check
    if (ret == 0)
    {
        mValid = true;
        if (!mDiscData.mTracks.isEmpty() && (mDiscData.mLenInMs > 0))
        {
            for (const auto& t : mDiscData.mTracks)
            {
                if ((t.mEndMs == 0)
                    || t.mAudioFile.isEmpty()
                    || ((t.mNo < 1) || (t.mNo > mDiscData.mTracks.size())))
                {
                    mValid = false;
                    ret = -1;
                    break;
                }
            }
        }
        else
        {
            mValid = false;
            ret = -1;
        }
    }
    else
    {
        mValid = false;
    }

    return ret;
}

//--------------------------------------------------------------------------
//! @brief      get track count
//!
//! @return     number of tracks
//--------------------------------------------------------------------------
int CueParser::trackCount() const
{
    return mDiscData.mTracks.size();
}

//--------------------------------------------------------------------------
//! @brief      git disc title
//!
//! @return     title as string
//--------------------------------------------------------------------------
const QString &CueParser::discTitle() const
{
    return mDiscData.mTitle;
}

//--------------------------------------------------------------------------
//! @brief      get disc artist / performer
//!
//! @return     artist as string
//--------------------------------------------------------------------------
const QString &CueParser::discPerfromer() const
{
    return mDiscData.mPerformer;
}

//--------------------------------------------------------------------------
//! @brief      get disc audio length in ms
//!
//! @return     length in ms
//--------------------------------------------------------------------------
uint32_t CueParser::discLength() const
{
    return mDiscData.mLenInMs;
}

//--------------------------------------------------------------------------
//! @brief      get complete track information
//!
//! @param[in]  no    track number, starts with 1
//!
//! @return     track information or empty struct
//--------------------------------------------------------------------------
const CueParser::Track &CueParser::track(int no) const
{
    if ((no <= mDiscData.mTracks.size()) && (no > -1))
    {
        // not index based
        return mDiscData.mTracks.at(no - 1);
    }
    else
    {
        return mEmptyTrack;
    }
}

//--------------------------------------------------------------------------
//! @brief      sanity check in Disc structure
//!
//! @return     true if valid; false if not
//--------------------------------------------------------------------------
bool CueParser::isValid() const
{
    return mValid;
}
