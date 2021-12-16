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
#include <QObject>
#include <QProcess>

class CCliProcess : public QProcess
{
    Q_OBJECT
public:
    CCliProcess(QObject* parent = nullptr);
    void run(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode = ReadWrite);
    bool busy() const;

private slots:
    void extractPercent();

signals:
    void progress(int);

protected:
    QString mLog;
};
