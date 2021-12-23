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
//! @brief      This class describes the atracdenc encoder handling.
//------------------------------------------------------------------------------
class CXEnc : public CCliProcess
{
    Q_OBJECT
    /// program executable path
#ifdef Q_OS_WIN
    static constexpr const char* XENC_CLI = "toolchain/atracdenc.exe";
#elif defined Q_OS_MAC
    static constexpr const char* XENC_CLI = "toolchain/atracdenc";
#else
    // hopefully in path
    static constexpr const char* XENC_CLI = "atracdenc";
#endif

    /// Sony WAVE format
    static constexpr uint32_t WAVE_FORMAT_SONY_SCX = 624;

    /// atrac3 header size in bytes
    static constexpr uint32_t ATRAC3_HEADER_SIZE = 96;

    /// block alignment for LP2 (used for DAO mode)
    static constexpr uint32_t ATRAC3_LP2_BLOCK_ALIGN = 384;

    /// block alignment for LP4
    static constexpr uint32_t ATRAC3_LP4_BLOCK_ALIGN = 192;

public:
    /// encoder commands
    enum class XEncCmd : uint8_t
    {
        NONE,           ///< nothing
        LP2_ENCODE,     ///< do LP2 encoding
        LP4_ENCODE,     ///< do LP4 encoding
        DAO_LP2_ENCODE  ///< do DAO LP2 encoding
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CXEnc(QObject *parent = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      start encoder
    //!
    //! @param[in]  cmd          The command
    //! @param[in]  tmpFileName  The temporary file name
    //! @param[in]  trackLength  The track length
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start(XEncCmd cmd, const QString& tmpFileName, uint32_t trackLength);
    
    //--------------------------------------------------------------------------
    //! @brief      start the encoder
    //!
    //! @param[in]  cmd         The command
    //! @param[in]  queue       The queue
    //! @param[in]  discLength  The disc length
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start(XEncCmd cmd, const c2n::TransferQueue& queue, uint32_t discLength);

protected:
    //--------------------------------------------------------------------------
    //! @brief      create atrac3 WAVE header
    //!
    //! @param      waveFile  The wave file
    //! @param[in]  cmd       The command
    //! @param[in]  dataSz    The data size
    //! @param[in]  length    The length
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int atrac3WaveHeader(QFile& waveFile, XEncCmd cmd, size_t dataSz, int length);
    
    //--------------------------------------------------------------------------
    //! @brief      Splits an atrac 3.
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int splitAtrac3();

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
    void fileDone(bool);

protected:
    /// current command
    XEncCmd mCurrCmd;

    /// file name for atrac3 output
    QString mAtracFileName;

    /// files to be handled
    c2n::TransferQueue mQueue;
    
    /// disc- / file - length
    uint32_t mLength;
};
