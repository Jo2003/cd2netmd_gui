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

    params << "-y" << "-i" << srcFileName;

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

    return start(params);
}

//--------------------------------------------------------------------------
//! @brief      start encoder with params
//!
//! @param[in]  params the call parameters
//! @param[in]  nativeArgs optional native arguments
//!
//! @return     0 on success
//--------------------------------------------------------------------------
int CFFMpeg::start(const QStringList& params, const QString& nativeArgs)
{
#ifdef Q_OS_MAC
    // app folder
    QString sAppDir = QApplication::applicationDirPath();

    // find bundle dir ...
    QRegExp rx("^(.*)/MacOS");
    if (rx.indexIn(sAppDir) > -1)
    {
       // found section --> create path names ...
       QString encTool = QString("%1/%2").arg(sAppDir).arg(FFMPEG_CLI);
       qInfo() << encTool << params;
       run(encTool, params);
   }
#else
    qInfo() << FFMPEG_CLI << params << nativeArgs;
    run(FFMPEG_CLI, params, ReadWrite, nativeArgs);
#endif
    return 0;
}

//--------------------------------------------------------------------------
//! @brief      concat audio files to supported wave file
//!
//! @param[in]  sources  The source file names
//! @param[in]  target   The target file name
//!
//! @return     0 on success
//--------------------------------------------------------------------------
int CFFMpeg::concatFiles(const QStringList& sources, const QString& target)
{
    // ffmpeg -i input1.wav -i input2.wav -i input3.wav -i input4.wav
    // -filter_complex '[0:0][1:0][2:0][3:0]concat=n=4:v=0:a=1[out]'
    // -map '[out]' output.wav

    const char* concatTmpl1 = "[%1:0]";
    const char* concatTmpl2 = R"("%1concat=n=%2:v=0:a=1[out]")";
    QString concatComplex;
    int i = 0;
    QStringList params;

    for(const auto& s : sources)
    {
        concatComplex += QString(concatTmpl1).arg(i);
        params << "-i" << s;
        i++;
    }

    // finish complex filter string
    concatComplex = QString(concatTmpl2).arg(concatComplex).arg(sources.size());

    // convert to wave, 44.1kHz, 16 bit
    params << "-y";
    params << "-acodec" << "pcm_s16le";
    params << "-ar" << "44100";
    params << "-ac" << "2";
    params << "-filter_complex";
#ifdef Q_OS_WIN
    concatComplex += R"( -map "[out]" -f wav ")" + target + R"(")";
#else
    params << concatComplex << "-map" << R"("[out]")" << "-f" << "wav" << target;
    concatComplex = "";
#endif
    return start(params, concatComplex);
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

    if ((length != -1) && (currPos != -1) && (length != 0))
    {
        emit progress((currPos * 100) / length);
    }
}
