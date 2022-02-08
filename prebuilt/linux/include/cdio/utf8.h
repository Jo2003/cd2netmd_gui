/*
    Copyright (C) 2008, 2012, 2014 Rocky Bernstein <rocky@gnu.org>
    Copyright (C) 2006 Burkhard Plaum <plaum@ipf.uni-stuttgart.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file utf8.h 
 *
 *  \brief UTF-8 support
 */


#include <cdio/types.h>

/** \brief Opaque characterset converter
 */

typedef struct cdio_charset_coverter_s cdio_charset_coverter_t;

/** \brief Create a charset converter
 *  \param src_charset Source charset
 *  \param dst_charset Destination charset
 *  \returns A newly allocated charset converter
 */

cdio_charset_coverter_t *
cdio_charset_converter_create(const char * src_charset,
                              const char * dst_charset);

/** \brief Destroy a characterset converter
 *  \param cnv A characterset converter
 */

void cdio_charset_converter_destroy(cdio_charset_coverter_t*cnv);

/** \brief Convert a string from one character set to another
 *  \param cnv A charset converter
 *  \param src Source string
 *  \param src_len Length of source string
 *  \param dst Returns destination string
 *  \param dst_len If non NULL, returns the length of the destination string
 *  \returns true if conversion was sucessful, false else.
 *
 *  The destination string must be freed by the caller with cdio_free().
 *  If you pass -1 for src_len, strlen() will be used.
 */

bool cdio_charset_convert(cdio_charset_coverter_t*cnv,
                          char * src, int src_len,
                          char ** dst, int * dst_len);

/** \brief Convert a string from UTF-8 to another charset
 *  \param src Source string (0 terminated)
 *  \param dst Returns destination string
 *  \param dst_len If non NULL, returns the length of the destination string
 *  \param dst_charset The characterset to convert to
 *  \returns true if conversion was sucessful, false else.
 *
 *  This is a convenience function, which creates a charset converter,
 *  converts one string and destroys the charset converter.
 *
 *  The destination string must be freed by the caller with cdio_free().
 */


bool cdio_charset_from_utf8(cdio_utf8_t * src, char ** dst,
                            int * dst_len, const char * dst_charset);

/** \brief Convert a string from another charset to UTF-8 
 *  \param src Source string
 *  \param src_len Length of the source string
 *  \param dst Returns destination string (0 terminated)
 *  \param src_charset The characterset to convert from
 *  \returns true if conversion was sucessful, false else.
 *
 *  This is a convenience function, which creates a charset converter,
 *  converts one string and destroys the charset converter.
 *
 *  The destination string must be freed by the caller with cdio_free().
 *  If you pass -1 for src_len, strlen() will be used.
 */


bool cdio_charset_to_utf8(const char *src, size_t src_len, cdio_utf8_t **dst,
                          const char * src_charset);

#ifdef _WIN32
/** \brief Convert an UTF8 string to UTF-16 (allocate returned string)
 *  \param str Source string
 *  \returns NULL if the conversion was unsuccesful. Caller must free the
 *  returned string with cdio_free().
 *  This is a convenience function available on Windows platforms only.
 */
wchar_t* cdio_utf8_to_wchar(const char* str);

#include <stdio.h> /* for FILE */
/** \brief Provides an UTF-8 compliant version of fopen for Windows
 *  The parameters and return value are the same as fopen().
 */
FILE* fopen_utf8(const char* filename, const char* mode);
#endif
