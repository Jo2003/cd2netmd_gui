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
#include "helpers.h"
#include <QTextCodec>
#include "defines.h"
#include "mdtitle.h"

static QByteArray s_Encoded;

static QMap<QString, QString> s_UmlautEnc = {
    {"à", "a"},
    {"À", "A"},
    {"á", "a"},
    {"Á", "A"},
    {"â", "a"},
    {"Â", "A"},
    {"å", "a"},
    {"Ã", "A"},
    {"ä", "ae"},
    {"Ä", "Ae"},

    {"æ", "ae"},
    {"Æ", "Ae"},

    {"ç", "c"},
    {"Ç", "C"},

    {"ê", "e"},
    {"Ê", "E"},
    {"é", "e"},
    {"É", "E"},
    {"ë", "e"},
    {"Ë", "E"},
    {"è", "e"},
    {"È", "E"},

    {"ï", "i"},
    {"Ï", "I"},
    {"í", "i"},
    {"Í", "I"},
    {"î", "i"},
    {"Î", "I"},
    {"ì", "i"},
    {"Ì", "I"},

    {"ñ", "n"},
    {"Ñ", "N"},

    {"œ", "oe"},
    {"Œ", "Oe"},

    {"ö", "oe"},
    {"Ö", "Oe"},
    {"ô", "o"},
    {"Ô", "O"},
    {"ò", "o"},
    {"Ò", "O"},
    {"ó", "o"},
    {"Ó", "O"},
    {"õ", "o"},
    {"Õ", "O"},
    {"ø", "oe"},
    {"Ø", "Oe"},

    {"š", "s"},
    {"Š", "S"},

    {"ú", "u"},
    {"Ú", "U"},
    {"ù", "u"},
    {"Ù", "U"},
    {"ü", "ue"},
    {"Ü", "Ue"},
    {"û", "u"},
    {"Û", "U"},

    {"ý", "y"},
    {"Ý", "Y"},
    {"ÿ", "y"},
    {"Ÿ", "Y"},

    {"ž", "z"},
    {"Ž", "Z"},

    {"Ð", "D"},
    {"ß", "ss"}
};

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

QString &deUmlaut(QString &s)
{
    for (const auto& k : s_UmlautEnc.keys())
    {
        s.replace(k, s_UmlautEnc.value(k));
    }
    return s;
}

const char *utf8ToMd(const QString& from)
{
    QString tmpStr = from;

    // encode all accents / umlaut to safe tokens
    deUmlaut(tmpStr);

    tmpStr = sanitizeHalfWidthTitle(tmpStr);

    QTextCodec *pCodec = QTextCodec::codecForName("Shift_JIS");
    QTextEncoder *pEnc = pCodec->makeEncoder();
    if (pEnc)
    {
        s_Encoded = pEnc->fromUnicode(tmpStr);
        delete pEnc;
    }
    else
    {
        s_Encoded = tmpStr.toLatin1();
    }

    return static_cast<const char*>(s_Encoded);
}

QString mdToUtf8(const QByteArray& ba)
{
    QString ret;
    QTextCodec *pCodec = QTextCodec::codecForName("Shift_JIS");
    QTextDecoder *pDec = pCodec->makeDecoder();

    if (pDec)
    {
        ret = pDec->toUnicode(ba);
        delete pDec;
    }
    else
    {
        ret = QString::fromLatin1(ba);
    }
    return ret;
}
