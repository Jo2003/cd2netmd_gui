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
    QRegExp rxpTitle("^TITLE\\s+(.*)$");
    QRegExp rxpPerf("^PERFORMER\\s+(.*)$");
    QRegExp rxpTrack("^TRACK\\s+([0-9]+)\\s+([A-Za-z]+)$");
    QRegExp rxpIndex("^INDEX\\s+01\\s+([0-9:]+)$");
    QRegExp rxpFile("^FILE\\s+(.*)\\s+([A-Za-z]+)$");
    QRegExp rxpUse("^(TITLE|PERFORMER|TRACK|INDEX\\s+01|FILE).*$");

    QFile cueFile(cueFileName);
    QFileInfo cfi(cueFileName);

    try
    {
        if (!cueFile.open(QIODevice::ReadOnly))
        {
            mCueThrow(-1, "Can't open cue file " << cueFileName);
        }

        bool inTrack = false;
        Track track;
        QString file;
        TrackType ftype = DATA, ttype = DATA;
        mDiscData = {"", "", 0, {}};
        QTextStream in(&cueFile);
        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();

            // check for needed information
            if (rxpUse.indexIn(line) > -1)
            {
                if (rxpFile.indexIn(line) > -1)
                {
                    file = rxpFile.cap(1).replace(QChar('"'), "");

                    // remove any directory part
                    QFileInfo afi(file);
                    file = afi.fileName();
                    ftype = (rxpFile.cap(2).toUpper() == "WAVE") ? AUDIO : DATA;
                }
                else if (rxpTitle.indexIn(line) > -1)
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
                else if (rxpPerf.indexIn(line) > -1)
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
                else if (rxpTrack.indexIn(line) > -1)
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
                        track = {-1, DATA, "", "", "", 0, 0};
                    }

                    inTrack = true;
                    track.mNo = rxpTrack.cap(1).toUInt();
                    ttype = (rxpTrack.cap(2).toUpper() == "AUDIO") ? AUDIO : DATA;
                }
                else if (rxpIndex.indexIn(line) > -1)
                {
                    // file mentioned before INDEX 01 is the track file
                    track.mAudioFile = file;
                    track.mType = ((ttype == ftype) && (ftype == AUDIO)) ? AUDIO : DATA;

                    QString idx = rxpIndex.cap(1);
                    auto idxs = idx.split(QChar(':'));
                    if (idxs.size() == 3)
                    {
                        uint32_t msec = idxs.at(0).toUInt() * 60 * 1000; // minutes
                        msec += idxs.at(1).toUInt() * 1000;              // seconds
                        msec += idxs.at(2).toUInt() * 10;                // 100ts of seconds
                        track.mStartMs = msec;

                        // we must mark the end of the former track
                        if (!mDiscData.mTracks.isEmpty())
                        {
                            Track& lastTrack = mDiscData.mTracks[mDiscData.mTracks.size() - 1];
                            if (msec)
                            {
                                lastTrack.mEndMs = msec;
                            }
                            else
                            {
                                uint32_t conv = 0;
                                int length = 0;
                                if (audio::checkAudioFile(cfi.canonicalPath() + "/" + lastTrack.mAudioFile, conv, length) == 0)
                                {
                                    lastTrack.mEndMs = static_cast<uint32_t>(length);
                                }
                            }
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

            uint32_t conv = 0;
            int length = 0;
            if (audio::checkAudioFile(cfi.canonicalPath() + "/" + track.mAudioFile, conv, length) == 0)
            {
                track.mEndMs = static_cast<uint32_t>(length);
            }

            // store track
            mDiscData.mTracks.append(track);
        }

        // compute disc length
        for (const auto& t : mDiscData.mTracks)
        {
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
const CueParser::Track &CueParser::track(int no)
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
