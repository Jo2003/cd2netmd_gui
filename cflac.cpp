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
#include "cflac.h"
#include "helpers.h"
#include <QApplication>

CFlac::CFlac(QObject *parent)
    : CCliProcess(parent)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CFlac::finishCopy);
}

//--------------------------------------------------------------------------
//! @brief      start encoder
//!
//! @param[in]  srcFileName  The source file name
//! @param[in]  trgFileName  The target file name
//!
//! @return     0 on success
//--------------------------------------------------------------------------
int CFlac::start(const QString& srcFileName, const QString trgFileName)
{
    QStringList params;
    mLog.clear();

    params << "-d" << srcFileName << "-o" << trgFileName;
	
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
    qInfo() << FLAC_CLI << params;
    run(FLAC_CLI, params);
#endif
    return 0;
}

void CFlac::finishCopy(int exitCode, ExitStatus exitStatus)
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
