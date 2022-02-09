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
#include "cffmpeg.h"
#include "helpers.h"
#include <QApplication>
#include <audio.h>

CFFMpeg::CFFMpeg(QObject *parent)
    : CCliProcess(parent)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CFFMpeg::finishCopy);
}

//--------------------------------------------------------------------------
//! @brief      start encoder
//!
//! @param[in]  srcFileName  The source file name
//! @param[in]  trgFileName  The target file name
//! @param[in]  conversion   The conversion settings
//!
//! @return     0 on success
//--------------------------------------------------------------------------
int CFFMpeg::start(const QString& srcFileName, const QString trgFileName, const uint32_t& conversion)
{
    QStringList params;
    mLog.clear();

    params << "-i" << srcFileName;

    if (conversion & audio::CONV_BPS)
    {
        params << "-acodec" << "pcm_s16le";
    }

    if (conversion & audio::CONV_SAMPLERATE)
    {
        params << "-ar" << "44100";
    }

    if (conversion & audio::CONV_CHANNELS)
    {
        params << "-ac" << "2";
    }

    params << trgFileName;
	
#ifdef Q_OS_MAC
	// app folder
    QString sAppDir = QApplication::applicationDirPath();
	
    // find bundle dir ...
    QRegExp rx("^(.*)/MacOS");
    if (rx.indexIn(sAppDir) > -1)
    {
       // found section --> create path names ...
       QString encTool = QString("%1/%2").arg(sAppDir).arg(FLAC_CLI);
	   qInfo() << encTool << params;
	   run(encTool, params);
   }
#else
    qInfo() << FFMPEG_CLI << params;
    run(FFMPEG_CLI, params);
#endif
    return 0;
}

void CFFMpeg::finishCopy(int exitCode, ExitStatus exitStatus)
{
    if ((exitCode == 0) && (exitStatus == ExitStatus::NormalExit))
    {
        qInfo() << "File successfully decoded!";
    }

    if (!mLog.isEmpty())
    {
        qDebug() << static_cast<const char*>(mLog.toUtf8());
    }

    emit fileDone();
}

//--------------------------------------------------------------------------
//! @brief      extract percentage from log file
//--------------------------------------------------------------------------
void CFFMpeg::extractPercent()
{
    mLog += QString::fromUtf8(readAllStandardOutput());
    int length = -1, currPos = -1;

    QRegExp rxDuration(R"(Duration: ([0-9]+):([0-9]+):([0-9]+).*)");
    QRegExp rxPosition(R"(time=([0-9]+):([0-9]+):([0-9]+).*)");

    if (mLog.indexOf(rxDuration) > -1)
    {
        // hours
        length += rxDuration.cap(1).toInt() * 3600;

        // minutes
        length += rxDuration.cap(2).toInt() * 60;

        // seconds
        length += rxDuration.cap(3).toInt();
    }

    if (mLog.lastIndexOf(rxPosition) > -1)
    {
        // hours
        currPos += rxPosition.cap(1).toInt() * 3600;

        // minutes
        currPos += rxPosition.cap(2).toInt() * 60;

        // seconds
        currPos += rxPosition.cap(3).toInt();
    }

    if ((length != -1) && (currPos != -1))
    {
        emit progress((currPos * 100) / length);
    }
}
