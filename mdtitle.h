/**
 * Copyright (C) 2022 Jo2003 (olenka.joerg@gmail.com)
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
#include <QString>

//------------------------------------------------------------------------------
//! @brief      Gets the half width title length.
//!
//! @param[in]  title  The title
//!
//! @return     The half width title length.
//------------------------------------------------------------------------------
size_t getHalfWidthTitleLength(const QString& title);

//------------------------------------------------------------------------------
//! @brief      check / fix full width title
//!
//! @param[in]  title  The title
//!
//! @return     The checked / fixed string.
//------------------------------------------------------------------------------
QString sanitizeFullWidthTitle(const QString& title, bool justRemap = true);

//------------------------------------------------------------------------------
//! @brief      check / fix half width title
//!
//! @param[in]  title  The title
//!
//! @return     The checked / fixed string.
//------------------------------------------------------------------------------
QString sanitizeHalfWidthTitle(const QString& title);

//------------------------------------------------------------------------------
//! @brief      split string into unicode tokens
//!
//! @param[in]  src   The source string
//!
//! @return     string list with unicode tokens
//------------------------------------------------------------------------------
QStringList unicodeSplit(const QString& src);
