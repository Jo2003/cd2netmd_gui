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
#include <QRegExp>
#include "ccliprocess.h"
#include "defines.h"

CCliProcess::CCliProcess(QObject *parent)
    :QProcess(parent)
{
    QProcess::setProcessChannelMode(ProcessChannelMode::MergedChannels);
    QProcess::setReadChannel(ProcessChannel::StandardOutput);
    connect(this, &QProcess::readyReadStandardOutput, this, &CCliProcess::extractPercent);
}

//--------------------------------------------------------------------------
//! @brief      run the process
//!
//! @param[in]  program    The program
//! @param[in]  arguments  The arguments
//! @param[in]  mode       The mode
//! @param[in]  nativeArgs optional native arguments (only used on Windows)
//--------------------------------------------------------------------------
void CCliProcess::run(const QString &program, const QStringList &arguments,
                      QIODevice::OpenMode mode, const QString& nativeArgs)
{
    mLog.clear();
    QProcess::setProgram(program);
    QStringList args = arguments;

#ifdef Q_OS_WIN
    // empty native args will clear last set args
    setNativeArguments(nativeArgs);
#else
    Q_UNUSED(nativeArgs)
#endif // Q_OS_WIN

    QProcess::setArguments(args);
    QProcess::start(mode);
    waitForStarted();
}

bool CCliProcess::busy() const
{
    return state() != QProcess::NotRunning;
}

void CCliProcess::extractPercent()
{
    mLog += QString::fromUtf8(readAllStandardOutput());
    int pos;

    if ((pos = mLog.lastIndexOf(QChar('%'))) > 0)
    {
        int startPos = mLog.lastIndexOf(QRegExp("[^0-9]+"), pos - 1);

        if (startPos > -1)
        {
            bool ok;
            int length  = (pos - 1) - startPos;
            int percent = mLog.mid(startPos + 1, length).toInt(&ok);

            if (ok)
            {
                emit progress(percent);
            }
        }
    }
}
