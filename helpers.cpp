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

QByteArray utf8ToMd(const QString& from)
{
    QString tmpStr = from;
    QByteArray baEncoded;

    // encode all accents / umlaut to safe tokens
    deUmlaut(tmpStr);

    tmpStr = sanitizeHalfWidthTitle(tmpStr);

    QTextCodec *pCodec = QTextCodec::codecForName("Shift_JIS");
    QTextEncoder *pEnc = pCodec->makeEncoder();
    if (pEnc)
    {
        baEncoded = pEnc->fromUnicode(tmpStr);
        delete pEnc;
    }
    else
    {
        baEncoded = tmpStr.toLatin1();
    }

    return baEncoded;
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
