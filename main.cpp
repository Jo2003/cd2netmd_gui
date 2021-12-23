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
#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QtGlobal>

static QFile g_logFile(QString("%1/cd2netmd_gui.log").arg(QDir::tempPath()));

void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString t = QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString message;
    QTextStream mss(&message);
    QTextStream oss(&g_logFile);

    if (context.file && context.function)
    {
        mss << context.file << ":" << context.line << "|" << context.function << "|";
    }

    mss << msg;

    message.replace("\r\n", "\n");
    message.replace("\n\n", "\n");
    message = message.trimmed();

    switch (type)
    {
    case QtDebugMsg:
        oss << t << "|DEBUG|" << message << "\n";
        break;
    case QtInfoMsg:
        oss << t << "|INFO|" << message << "\n";
        break;
    case QtWarningMsg:
        oss << t << "|WARNING|" << message << "\n";
        break;
    case QtCriticalMsg:
        oss << t << "|CRITICAL|" << message << "\n";
        break;
    case QtFatalMsg:
        oss << t << "|FATAL|" << message << "\n";
        abort();
    }
}

int main(int argc, char *argv[])
{
    int exitCode = 0;
    g_logFile.open(QIODevice::Text | QIODevice::Truncate | QIODevice::WriteOnly);
    qInstallMessageHandler(logger);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    exitCode = a.exec();
    g_logFile.close();
    return exitCode;
}
