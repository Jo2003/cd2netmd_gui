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
#include <QVector>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      This class describes the Mac drutil handling.
//------------------------------------------------------------------------------
class CDRUtil : public CCliProcess
{
    Q_OBJECT
    /// program executable path
    static constexpr const char* DRUTIL_CLI = "drutil";

public:
    /// CD Text needed information
    struct SCDText
    {
        QString mArtist;    ///< artist
        QString mTitle;     ///< title
    };

    /// type to send data
    using CDTextData = QVector<SCDText>;

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CDRUtil(QObject *parent = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      start encoder
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start();

private slots:
    //--------------------------------------------------------------------------
    //! @brief      Finishes a copy.
    //!
    //! @param[in]  exitCode    The exit code
    //! @param[in]  exitStatus  The exit status
    //--------------------------------------------------------------------------
    void finish(int exitCode, ExitStatus exitStatus);

signals:
    //--------------------------------------------------------------------------
    //! @brief      signals that current file was handled
    //!
    //! @param[in]  cdt  cd text data
    //--------------------------------------------------------------------------
    void fileDone(CDTextData cdt);

protected:
    //--------------------------------------------------------------------------
    //! @brief      parse output of drutil (xml)
    //!
    //! @param[in]  xmlData    xml data string
    //! @param[in]  cdtdata    buffer for CD-TEXT data
    //!
    //! @return 0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int parseXml(const QString& xmlData, CDTextData& cdtdata);
};
