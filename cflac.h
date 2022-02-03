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
#include "ccliprocess.h"
#include <QFile>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      This class describes the flac decoder handling.
//------------------------------------------------------------------------------
class CFlac : public CCliProcess
{
    Q_OBJECT
    /// program executable path
#ifdef Q_OS_WIN
    static constexpr const char* FLAC_CLI = "toolchain/flac.exe";
#elif defined Q_OS_MAC
    static constexpr const char* FLAC_CLI = "flac";
#else
    // hopefully in path
    static constexpr const char* FLAC_CLI = "flac";
#endif

public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CFlac(QObject *parent = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      start encoder
    //!
    //! @param[in]  srcFileName  The source file name
    //! @param[in]  trgFileName  The target file name
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start(const QString& srcFileName, const QString trgFileName);

private slots:
    //--------------------------------------------------------------------------
    //! @brief      Finishes a copy.
    //!
    //! @param[in]  exitCode    The exit code
    //! @param[in]  exitStatus  The exit status
    //--------------------------------------------------------------------------
    void finishCopy(int exitCode, ExitStatus exitStatus);

signals:
    //--------------------------------------------------------------------------
    //! @brief      signals that current file was handled
    //!
    //! @param[in]  <unnamed>  false
    //--------------------------------------------------------------------------
    void fileDone();
};
