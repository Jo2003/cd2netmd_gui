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
#include "utils.h"

int writeWaveHeader(QFile &wf, size_t byteCount)
{
    wf.write("RIFF", 4);                // 0
    putNum(byteCount + 44 - 8, wf, 4);  // 4
    wf.write("WAVEfmt ", 8);            // 8
    putNum(16, wf, 4);                  // 16
    putNum(1, wf, 2);                   // 20
    putNum(2, wf, 2);                   // 22
    putNum(44100, wf, 4);               // 24
    putNum(44100 * 2 * 2, wf, 4);       // 28
    putNum(4, wf, 2);                   // 32
    putNum(16, wf, 2);                  // 34
    wf.write("data", 4);                // 36
    putNum(byteCount, wf, 4);           // 40

    return 0;
}

int putNum(uint32_t num, QFile &f, size_t sz)
{
    unsigned int i;
    char c;

    for (i = 0; sz--; i++)
    {
        c = (num >> (i << 3)) & 0xff;
        if (f.write(&c, 1) != 1)
        {
            return -1;
        }
    }
    return 0;
}

QString utf8ToMd(const QString& from)
{
    QString tmpStr = from;
    tmpStr.replace("Ä", "Ae");
    tmpStr.replace("ä", "ae");
    tmpStr.replace("Ö", "Oe");
    tmpStr.replace("ö", "oe");
    tmpStr.replace("Ü", "Ue");
    tmpStr.replace("ü", "ue");
    tmpStr.replace("ß", "ss");

    iconv_t icv = iconv_open("US-ASCII//TRANSLIT//IGNORE", "UTF-8");
    return QString::fromStdString(cddb_str_iconv(icv, static_cast<const char*>(tmpStr.toUtf8())));
}

//------------------------------------------------------------------------------
//! @brief      convert string
//!
//! @param[in]  cd    conversion enum
//! @param[in]  in    string to convert
//! @param      out   converted string
//!
//! @return     converted string on success; unconverted in error case
//------------------------------------------------------------------------------
std::string cddb_str_iconv(iconv_t cd, const char *in)
{
    std::string ret = in;
    size_t inlen, outlen;
    int buflen, rc;
    int len;                    /* number of chars in buffer */
    char *buf;
    char *inbuf = strdup(in);
    char *inPtr = inbuf;

    if (inbuf != nullptr)
    {
        inlen = strlen(inbuf);
        buflen = 0;
        buf = NULL;
        do {
            outlen = inlen * 2;
            buflen += outlen;
            /* iconv() below changes the buf pointer:
             * - decrement to point at beginning of buffer before realloc
             * - re-increment to point at first free position after realloc
             */
            len = buflen - outlen;
            buf = (char*)realloc(buf - len, buflen) + len;
            if (buf == NULL) {
                /* XXX: report out of memory error */
                free(inbuf);
                return ret;
            }
            rc = iconv(cd, &inPtr, &inlen, &buf, &outlen);
            if ((rc == -1) && (errno != E2BIG)) {
                free(buf);
                free(inbuf);
                fprintf(stderr, "Error in character encoding!\n");
                fflush(stderr);
                return ret;       /* conversion failed */
            }
        } while (inlen != 0);
        len = buflen - outlen;
        buf -= len;                 /* reposition at begin of buffer */

        /* make a copy just big enough for the result */
        char *o = new char[len + 1];

        if (o != nullptr)
        {
            memcpy(o, buf, len);
            o[len] = '\0';
            ret = o;
            delete [] o;

            free(inbuf);

            return ret;
        }

        free(inbuf);
    }

    return ret;
}
