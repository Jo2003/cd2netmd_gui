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

int putNum(uint32_t num, QFile &f, size_t sz);

///
/// \brief UTF-8 to MiniDisc text
/// \param from string to convert
/// \return converted string
///
const char* utf8ToMd(const QString& from);

///
/// \brief mini disc text to utf-8
/// \param from SHIFT-JIS encoded text
/// \return unicode string
///
QString mdToUtf8(const QByteArray& ba);

///
/// \brief convert umlaut and accents
/// \param s string to convert
/// \return ref. to converted string
///
QString& deUmlaut(QString& s);
