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
#include <QFile>
#include <QString>
#include <cstdint>
#include <iconv.h>

int writeWaveHeader(QFile &wf, size_t byteCount);

int putNum(uint32_t num, QFile &f, size_t sz);

///
/// \brief UTF-8 to MiniDisc text
/// \param from string to convert
/// \return converted string
///
QString utf8ToMd(const QString& from);

//------------------------------------------------------------------------------
//! @brief      convert string
//!
//! @param[in]  cd    conversion enum
//! @param[in]  in    string to convert
//! @param      out   converted string
//!
//! @return     converted string on success; unconverted in error case
//------------------------------------------------------------------------------
std::string cddb_str_iconv(iconv_t cd, const char *in);
