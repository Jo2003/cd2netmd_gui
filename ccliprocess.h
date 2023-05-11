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

//------------------------------------------------------------------------------
//! @brief      This class describes a cli process.
//------------------------------------------------------------------------------
class CCliProcess : public QProcess
{
    Q_OBJECT
public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    CCliProcess(QObject* parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      run the process
    //!
    //! @param[in]  program    The program
    //! @param[in]  arguments  The arguments
    //! @param[in]  mode       The mode
    //! @param[in]  nativeArgs optional native arguments (only used on Windows)
    //--------------------------------------------------------------------------
    void run(const QString &program,
             const QStringList &arguments,
             QIODevice::OpenMode mode = ReadWrite,
             const QString& nativeArgs = "");
    
    //--------------------------------------------------------------------------
    //! @brief      check if process is running (busy)
    //!
    //! @return     true -> busy; false -> free
    //--------------------------------------------------------------------------
    bool busy() const;

private slots:
    //--------------------------------------------------------------------------
    //! @brief      extract percentage from log file
    //--------------------------------------------------------------------------
    virtual void extractPercent();

signals:
    //--------------------------------------------------------------------------
    //! @brief      signal progress in percent
    //!
    //! @param[in]  <unnamed>  percent value
    //--------------------------------------------------------------------------
    void progress(int);

protected:
    /// log content
    QString mLog;
};

