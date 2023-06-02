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
#include <QTimer>
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
    static constexpr const char* XENC_CLI = "atracdenc";
#else
    // hopefully in path
    static constexpr const char* XENC_CLI = "atracdenc";
#endif

    /// Sony WAVE format
    static constexpr uint32_t WAVE_FORMAT_SONY_SCX = 624;

    /// atrac3 header size in bytes
    static constexpr uint32_t ATRAC3_HEADER_SIZE = 96;

    /// atrac (SP) header size in bytes
    static constexpr uint32_t ATRAC_SP_HEADER_SIZE = 2048;

    /// block alignment for LP2 (used for DAO mode)
    static constexpr uint32_t ATRAC3_LP2_BLOCK_ALIGN = 384;

    /// block alignment for LP4
    static constexpr uint32_t ATRAC3_LP4_BLOCK_ALIGN = 192;

    /// block alignment for SP (mono)
    static constexpr uint32_t ATRAC_SP_BLOCK_ALIGN = 212;
    
    /// we always copy stereo
    static constexpr uint32_t AT_SP_STEREO_BLOCK_SIZE = ATRAC_SP_BLOCK_ALIGN * 2;

public:
    /// encoder commands
    enum class XEncCmd : uint8_t
    {
        NONE,           ///< nothing
        LP2_ENCODE,     ///< do LP2 encoding
        LP4_ENCODE,     ///< do LP4 encoding
        DAO_LP2_ENCODE, ///< do DAO LP2 encoding
        DAO_SP_ENCODE,  ///< do DAO SP encoding
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
    //! @param[in]  at3tool      optional path to alternate encoder
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start(XEncCmd cmd, const QString& tmpFileName, double trackLength, const QString& at3tool = "");
    
    //--------------------------------------------------------------------------
    //! @brief      start the encoder
    //!
    //! @param[in]  cmd         The command
    //! @param[in]  queue       The queue
    //! @param[in]  discLength  The disc length
    //! @param[in]  at3tool     optional path to alternate encoder
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int start(XEncCmd cmd, const c2n::TransferQueue& queue, double discLength, const QString& at3tool = "");

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
    //! @brief      create atrac1 (SP) header
    //!
    //! @param      aeaFile   The target file
    //! @param[in]  cmd       The command
    //! @param[in]  dataSz    The data size
    //! @param[in]  length    The length
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int atrac1Header(QFile& aeaFile, XEncCmd cmd, size_t dataSz, int length);
    
    //--------------------------------------------------------------------------
    //! @brief      Splits an atrac 3.
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int splitAtrac3();

    //--------------------------------------------------------------------------
    //! @brief      Splits an atrac 1 (SP).
    //!
    //! @return     0 on success
    //--------------------------------------------------------------------------
    int splitAtrac1();

protected slots:
    //--------------------------------------------------------------------------
    //! @brief      extract percentage from log file
    //--------------------------------------------------------------------------
    void extractPercent() override;

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
    double mLength;

    /// do we use the alternate encoder?
    bool mbAltEnc;

    /// stores current progress step
    int mProgressIt;

    /// used for external encoer for progress update
    QTimer mProgUpd;
};
