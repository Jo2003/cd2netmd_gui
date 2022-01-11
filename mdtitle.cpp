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
#include "mdtitle.h"
#include "defines.h"
#include <QString>
#include <QMap>
#include <QChar>
#include <QTextEncoder>

/*
 * This code was highly inspired by webminidisc! Many thanks for this!
 */

const static QMap<QString, QString> s_FullToHalfWidth = {
    {"－", "-"},
    {"ｰ", "-"},
    {"ァ", "ｧ"},
    {"ア", "ｱ"},
    {"ィ", "ｨ"},
    {"イ", "ｲ"},
    {"ゥ", "ｩ"},
    {"ウ", "ｳ"},
    {"ェ", "ｪ"},
    {"エ", "ｴ"},
    {"ォ", "ｫ"},
    {"オ", "ｵ"},
    {"カ", "ｶ"},
    {"ガ", "ｶﾞ"},
    {"キ", "ｷ"},
    {"ギ", "ｷﾞ"},
    {"ク", "ｸ"},
    {"グ", "ｸﾞ"},
    {"ケ", "ｹ"},
    {"ゲ", "ｹﾞ"},
    {"コ", "ｺ"},
    {"ゴ", "ｺﾞ"},
    {"サ", "ｻ"},
    {"ザ", "ｻﾞ"},
    {"シ", "ｼ"},
    {"ジ", "ｼﾞ"},
    {"ス", "ｽ"},
    {"ズ", "ｽﾞ"},
    {"セ", "ｾ"},
    {"ゼ", "ｾﾞ"},
    {"ソ", "ｿ"},
    {"ゾ", "ｿﾞ"},
    {"タ", "ﾀ"},
    {"ダ", "ﾀﾞ"},
    {"チ", "ﾁ"},
    {"ヂ", "ﾁﾞ"},
    {"ッ", "ｯ"},
    {"ツ", "ﾂ"},
    {"ヅ", "ﾂﾞ"},
    {"テ", "ﾃ"},
    {"デ", "ﾃﾞ"},
    {"ト", "ﾄ"},
    {"ド", "ﾄﾞ"},
    {"ナ", "ﾅ"},
    {"ニ", "ﾆ"},
    {"ヌ", "ﾇ"},
    {"ネ", "ﾈ"},
    {"ノ", "ﾉ"},
    {"ハ", "ﾊ"},
    {"バ", "ﾊﾞ"},
    {"パ", "ﾊﾟ"},
    {"ヒ", "ﾋ"},
    {"ビ", "ﾋﾞ"},
    {"ピ", "ﾋﾟ"},
    {"フ", "ﾌ"},
    {"ブ", "ﾌﾞ"},
    {"プ", "ﾌﾟ"},
    {"ヘ", "ﾍ"},
    {"ベ", "ﾍﾞ"},
    {"ペ", "ﾍﾟ"},
    {"ホ", "ﾎ"},
    {"ボ", "ﾎﾞ"},
    {"ポ", "ﾎﾟ"},
    {"マ", "ﾏ"},
    {"ミ", "ﾐ"},
    {"ム", "ﾑ"},
    {"メ", "ﾒ"},
    {"モ", "ﾓ"},
    {"ャ", "ｬ"},
    {"ヤ", "ﾔ"},
    {"ュ", "ｭ"},
    {"ユ", "ﾕ"},
    {"ョ", "ｮ"},
    {"ヨ", "ﾖ"},
    {"ラ", "ﾗ"},
    {"リ", "ﾘ"},
    {"ル", "ﾙ"},
    {"レ", "ﾚ"},
    {"ロ", "ﾛ"},
    {"ワ", "ﾜ"},
    {"ヲ", "ｦ"},
    {"ン", "ﾝ"},
    {"ー", "-"},
    {"ヮ", "ヮ"},
    {"ヰ", "ヰ"},
    {"ヱ", "ヱ"},
    {"ヵ", "ヵ"},
    {"ヶ", "ヶ"},
    {"ヴ", "ｳﾞ"},
    {"ヽ", "ヽ"},
    {"ヾ", "ヾ"},
    {"・", "･"},
    {"「", "｢"},
    {"」", "｣"},
    {"。", "｡"},
    {"、", "､"},
    {"！", "!"},
    {"＂", "\""},
    {"＃", "#"},
    {"＄", "$"},
    {"％", "%"},
    {"＆", "&"},
    {"＇", "'"},
    {"（", "("},
    {"）", ")"},
    {"＊", "*"},
    {"＋", "+"},
    {"，", ","},
    {"．", "."},
    {"／", "/"},
    {"：", ":"},
    {"；", ";"},
    {"＜", "<"},
    {"＝", "="},
    {"＞", ">"},
    {"？", "?"},
    {"＠", "@"},
    {"Ａ", "A"},
    {"Ｂ", "B"},
    {"Ｃ", "C"},
    {"Ｄ", "D"},
    {"Ｅ", "E"},
    {"Ｆ", "F"},
    {"Ｇ", "G"},
    {"Ｈ", "H"},
    {"Ｉ", "I"},
    {"Ｊ", "J"},
    {"Ｋ", "K"},
    {"Ｌ", "L"},
    {"Ｍ", "M"},
    {"Ｎ", "N"},
    {"Ｏ", "O"},
    {"Ｐ", "P"},
    {"Ｑ", "Q"},
    {"Ｒ", "R"},
    {"Ｓ", "S"},
    {"Ｔ", "T"},
    {"Ｕ", "U"},
    {"Ｖ", "V"},
    {"Ｗ", "W"},
    {"Ｘ", "X"},
    {"Ｙ", "Y"},
    {"Ｚ", "Z"},
    {"［", "["},
    {"＼", "\\"},
    {"］", "]"},
    {"＾", "^"},
    {"＿", "_"},
    {"｀", "`"},
    {"ａ", "a"},
    {"ｂ", "b"},
    {"ｃ", "c"},
    {"ｄ", "d"},
    {"ｅ", "e"},
    {"ｆ", "f"},
    {"ｇ", "g"},
    {"ｈ", "h"},
    {"ｉ", "i"},
    {"ｊ", "j"},
    {"ｋ", "k"},
    {"ｌ", "l"},
    {"ｍ", "m"},
    {"ｎ", "n"},
    {"ｏ", "o"},
    {"ｐ", "p"},
    {"ｑ", "q"},
    {"ｒ", "r"},
    {"ｓ", "s"},
    {"ｔ", "t"},
    {"ｕ", "u"},
    {"ｖ", "v"},
    {"ｗ", "w"},
    {"ｘ", "x"},
    {"ｙ", "y"},
    {"ｚ", "z"},
    {"｛", "{"},
    {"｜", "|"},
    {"｝", "}"},
    {"～", "~"},
    {"\u3000", " "},
    {"０", "0"},
    {"１", "1"},
    {"２", "2"},
    {"３", "3"},
    {"４", "4"},
    {"５", "5"},
    {"６", "6"},
    {"７", "7"},
    {"８", "8"},
    {"９", "9"},
    {"ぁ", "ｧ"},
    {"あ", "ｱ"},
    {"ぃ", "ｨ"},
    {"い", "ｲ"},
    {"ぅ", "ｩ"},
    {"う", "ｳ"},
    {"ぇ", "ｪ"},
    {"え", "ｴ"},
    {"ぉ", "ｫ"},
    {"お", "ｵ"},
    {"か", "ｶ"},
    {"が", "ｶﾞ"},
    {"き", "ｷ"},
    {"ぎ", "ｷﾞ"},
    {"く", "ｸ"},
    {"ぐ", "ｸﾞ"},
    {"け", "ｹ"},
    {"げ", "ｹﾞ"},
    {"こ", "ｺ"},
    {"ご", "ｺﾞ"},
    {"さ", "ｻ"},
    {"ざ", "ｻﾞ"},
    {"し", "ｼ"},
    {"じ", "ｼﾞ"},
    {"す", "ｽ"},
    {"ず", "ｽﾞ"},
    {"せ", "ｾ"},
    {"ぜ", "ｾﾞ"},
    {"そ", "ｿ"},
    {"ぞ", "ｿﾞ"},
    {"た", "ﾀ"},
    {"だ", "ﾀﾞ"},
    {"ち", "ﾁ"},
    {"ぢ", "ﾁﾞ"},
    {"っ", "ｯ"},
    {"つ", "ﾂ"},
    {"づ", "ﾂﾞ"},
    {"て", "ﾃ"},
    {"で", "ﾃﾞ"},
    {"と", "ﾄ"},
    {"ど", "ﾄﾞ"},
    {"な", "ﾅ"},
    {"に", "ﾆ"},
    {"ぬ", "ﾇ"},
    {"ね", "ﾈ"},
    {"の", "ﾉ"},
    {"は", "ﾊ"},
    {"ば", "ﾊﾞ"},
    {"ぱ", "ﾊﾟ"},
    {"ひ", "ﾋ"},
    {"び", "ﾋﾞ"},
    {"ぴ", "ﾋﾟ"},
    {"ふ", "ﾌ"},
    {"ぶ", "ﾌﾞ"},
    {"ぷ", "ﾌﾟ"},
    {"へ", "ﾍ"},
    {"べ", "ﾍﾞ"},
    {"ぺ", "ﾍﾟ"},
    {"ほ", "ﾎ"},
    {"ぼ", "ﾎﾞ"},
    {"ぽ", "ﾎﾟ"},
    {"ま", "ﾏ"},
    {"み", "ﾐ"},
    {"む", "ﾑ"},
    {"め", "ﾒ"},
    {"も", "ﾓ"},
    {"ゃ", "ｬ"},
    {"や", "ﾔ"},
    {"ゅ", "ｭ"},
    {"ゆ", "ﾕ"},
    {"ょ", "ｮ"},
    {"よ", "ﾖ"},
    {"ら", "ﾗ"},
    {"り", "ﾘ"},
    {"る", "ﾙ"},
    {"れ", "ﾚ"},
    {"ろ", "ﾛ"},
    {"わ", "ﾜ"},
    {"を", "ｦ"},
    {"ん", "ﾝ"},
    {"ゎ", "ヮ"},
    {"ゐ", "ヰ"},
    {"ゑ", "ヱ"},
    {"ゕ", "ヵ"},
    {"ゖ", "ヶ"},
    {"ゔ", "ｳﾞ"},
    {"ゝ", "ヽ"},
    {"ゞ", "ヾ"}
};

const static QMap<QString, QString> s_HalfToFullWidth = {
    {"!", "！"},
    {"\"", "＂"},
    {"#", "＃"},
    {"$", "＄"},
    {"%", "％"},
    {"&", "＆"}, 
    {"'", "＇"},
    {"(", "（"},
    {")", "）"},
    {"*", "＊"},
    {"+", "＋"},
    {",", "，"},
    {"-", "－"},
    {".", "．"},
    {"/", "／"},
    {":", "："},
    {";", "；"},
    {"<", "＜"},
    {"=", "＝"},
    {">", "＞"},
    {"?", "？"},
    {"@", "＠"},
    {"A", "Ａ"},
    {"B", "Ｂ"},
    {"C", "Ｃ"},
    {"D", "Ｄ"},
    {"E", "Ｅ"},
    {"F", "Ｆ"},
    {"G", "Ｇ"},
    {"H", "Ｈ"},
    {"I", "Ｉ"},
    {"J", "Ｊ"},
    {"K", "Ｋ"},
    {"L", "Ｌ"},
    {"M", "Ｍ"},
    {"N", "Ｎ"},
    {"O", "Ｏ"},
    {"P", "Ｐ"},
    {"Q", "Ｑ"},
    {"R", "Ｒ"},
    {"S", "Ｓ"},
    {"T", "Ｔ"},
    {"U", "Ｕ"},
    {"V", "Ｖ"},
    {"W", "Ｗ"},
    {"X", "Ｘ"},
    {"Y", "Ｙ"},
    {"Z", "Ｚ"},
    {"[", "［"},
    {"\\", "＼"},
    {"]", "］"},
    {"^", "＾"},
    {"_", "＿"},
    {"`", "｀"},
    {"a", "ａ"},
    {"b", "ｂ"},
    {"c", "ｃ"},
    {"d", "ｄ"},
    {"e", "ｅ"},
    {"f", "ｆ"},
    {"g", "ｇ"},
    {"h", "ｈ"},
    {"i", "ｉ"},
    {"j", "ｊ"},
    {"k", "ｋ"},
    {"l", "ｌ"},
    {"m", "ｍ"},
    {"n", "ｎ"},
    {"o", "ｏ"},
    {"p", "ｐ"},
    {"q", "ｑ"},
    {"r", "ｒ"},
    {"s", "ｓ"},
    {"t", "ｔ"},
    {"u", "ｕ"},
    {"v", "ｖ"},
    {"w", "ｗ"},
    {"x", "ｘ"},
    {"y", "ｙ"},
    {"z", "ｚ"},
    {"{", "｛"},
    {"|", "｜"},
    {"}", "｝"},
    {"~", "～"},
    {" ", "\u3000"},
    {"0", "０"},
    {"1", "１"},
    {"2", "２"},
    {"3", "３"},
    {"4", "４"},
    {"5", "５"},
    {"6", "６"},
    {"7", "７"},
    {"8", "８"},
    {"9", "９"},
    {"ｧ", "ァ"},
    {"ｱ", "ア"},
    {"ｨ", "ィ"},
    {"ｲ", "イ"},
    {"ｩ", "ゥ"},
    {"ｳ", "ウ"},
    {"ｪ", "ェ"},
    {"ｴ", "エ"},
    {"ｫ", "ォ"},
    {"ｵ", "オ"},
    {"ｶ", "カ"},
    {"ｶﾞ", "ガ"},
    {"ｷ", "キ"},
    {"ｷﾞ", "ギ"},
    {"ｸ", "ク"},
    {"ｸﾞ", "グ"},
    {"ｹ", "ケ"},
    {"ｹﾞ", "ゲ"},
    {"ｺ", "コ"},
    {"ｺﾞ", "ゴ"},
    {"ｻ", "サ"},
    {"ｻﾞ", "ザ"},
    {"ｼ", "シ"},
    {"ｼﾞ", "ジ"},
    {"ｽ", "ス"},
    {"ｽﾞ", "ズ"},
    {"ｾ", "セ"},
    {"ｾﾞ", "ゼ"},
    {"ｿ", "ソ"},
    {"ｿﾞ", "ゾ"},
    {"ﾀ", "タ"},
    {"ﾀﾞ", "ダ"},
    {"ﾁ", "チ"},
    {"ﾁﾞ", "ヂ"},
    {"ｯ", "ッ"},
    {"ﾂ", "ツ"},
    {"ﾂﾞ", "ヅ"},
    {"ﾃ", "テ"},
    {"ﾃﾞ", "デ"},
    {"ﾄ", "ト"},
    {"ﾄﾞ", "ド"},
    {"ﾅ", "ナ"},
    {"ﾆ", "ニ"},
    {"ﾇ", "ヌ"},
    {"ﾈ", "ネ"},
    {"ﾉ", "ノ"},
    {"ﾊ", "ハ"},
    {"ﾊﾞ", "バ"},
    {"ﾊﾟ", "パ"},
    {"ﾋ", "ヒ"},
    {"ﾋﾞ", "ビ"},
    {"ﾋﾟ", "ピ"},
    {"ﾌ", "フ"},
    {"ﾌﾞ", "ブ"},
    {"ﾌﾟ", "プ"},
    {"ﾍ", "ヘ"},
    {"ﾍﾞ", "ベ"},
    {"ﾍﾟ", "ペ"},
    {"ﾎ", "ホ"},
    {"ﾎﾞ", "ボ"},
    {"ﾎﾟ", "ポ"},
    {"ﾏ", "マ"},
    {"ﾐ", "ミ"},
    {"ﾑ", "ム"},
    {"ﾒ", "メ"},
    {"ﾓ", "モ"},
    {"ｬ", "ャ"},
    {"ﾔ", "ヤ"},
    {"ｭ", "ュ"},
    {"ﾕ", "ユ"},
    {"ｮ", "ョ"},
    {"ﾖ", "ヨ"},
    {"ﾗ", "ラ"},
    {"ﾘ", "リ"},
    {"ﾙ", "ル"},
    {"ﾚ", "レ"},
    {"ﾛ", "ロ"},
    {"ﾜ", "ワ"},
    {"ｦ", "ヲ"},
    {"ﾝ", "ン"},
    {"ｰ", "ー"},
    {"ヮ", "ヮ"},
    {"ヰ", "ヰ"},
    {"ヱ", "ヱ"},
    {"ヵ", "ヵ"},
    {"ヶ", "ヶ"},
    {"ｳﾞ", "ヴ"},
    {"ヽ", "ヽ"},
    {"ヾ", "ヾ"},
    {"･", "・"},
    {"｢", "「"},
    {"｣", "」"},
    {"｡", "。"},
    {"､", "、"}
};

const static QString s_HandakutenPossible = "はひふへほハヒフヘホ";
const static QString s_DakutenPossible = "かきくけこさしすせそたちつてとカキクケコサシスセソタチツテト" + s_HandakutenPossible;
const static QString s_MultiByteChars = "ガギグゲゴザジズゼゾダヂヅデドバパビピブプベペボポヮヰヱヵヶヴヽヾがぎぐげござじずぜぞだぢづでどばぱびぴぶぷべぺぼぽゎゐゑゕゖゔゝゞ";

size_t getHalfWidthTitleLength(const QString& title)
{
    size_t      length   = title.size();
    QStringList uniChars = unicodeSplit(title);
    for (const auto& uc : uniChars)
    {
        if (s_MultiByteChars.contains(uc))
        {
            length ++;
        }
    }
    return length;
}

QString sanitizeFullWidthTitle(const QString &title, bool justRemap)
{
    QString out;

    QStringList vals     = s_HalfToFullWidth.values();
    QStringList uniChars = unicodeSplit(title);

    for (const auto& uc : uniChars)
    {
        if (s_HalfToFullWidth.contains(uc))
        {
            out += s_HalfToFullWidth.value(uc);
        }
        else
        {
            out += uc;
        }
    }

    if (justRemap) return out;

    QTextCodec *pCodec = QTextCodec::codecForName("Shift_JIS");
    QTextEncoder *pEnc = pCodec->makeEncoder();
    QTextDecoder *pDec = pCodec->makeDecoder();
    if (pEnc && pDec)
    {
        QByteArray ba = pEnc->fromUnicode(out);

        if (out != pDec->toUnicode(ba))
        {
            qDebug() << "Encode <-> decode test doesn't match -> fallback!";
            out = title;
        }

        if (ba.size() != (title.size() * 2))
        {
            qDebug() << "Encoded size doesn't match -> fallback!";
            out = title;
        }
    }

    if (pEnc)
    {
        delete pEnc;
    }

    if (pDec)
    {
        delete pDec;
    }
    return out;
}

QString sanitizeHalfWidthTitle(const QString& title)
{
    enum CharType {
        normal,
        dakuten,
        handakuten,
    };

    //'Flatten' all the characters followed by the (han)dakuten character into one
    QString dakutenFix;
    QString fwTitle;
    CharType type    = CharType::normal;
    QString tmpTitle = sanitizeFullWidthTitle(title);

    // revers title
    std::reverse(tmpTitle.begin(), tmpTitle.end());

    for (const auto& c : tmpTitle)
    {
        switch(type)
        {
        case CharType::dakuten:
            if (s_DakutenPossible.contains(c))
            {
                dakutenFix += QChar(c.cell() + 1);
                type = CharType::normal;
                break;
            }
            // fall through
        case CharType::handakuten:
            if (s_HandakutenPossible.contains(c))
            {
                dakutenFix += QChar(c.cell() + 2);
                type = CharType::normal;
                break;
            }
            // fall through
        case CharType::normal:
            switch(c.unicode())
            {
            case 0x309B:
            case 0x3099:
            case 0xFF9E:
                qDebug() << "Found dakuten!";
                type = CharType::dakuten;
                break;
            case 0x309C:
            case 0x309A:
            case 0xFF9F:
                qDebug() << "Found handakuten!";
                type = CharType::handakuten;
                break;
            default:
                type = CharType::normal;
                dakutenFix += c;
                break;
            }
        }
    }

    // reverse dakutenfix
    std::reverse(dakutenFix.begin(), dakutenFix.end());

    QStringList hwVals = s_FullToHalfWidth.values();
    dakutenFix = dakutenFix.normalized(QString::NormalizationForm_D);

    for (uint16_t i = 0x0300; i <= 0x036f; i++)
    {
        dakutenFix.replace(QChar(i), "");
    }
    tmpTitle = "";

    QStringList uniChars = unicodeSplit(dakutenFix);

    for (const auto& uc : uniChars)
    {
        if (s_FullToHalfWidth.contains(uc))
        {
            tmpTitle += s_FullToHalfWidth.value(uc);
        }
        else if (hwVals.contains(uc) || (uc[0].cell() < 0x7f))
        {
            tmpTitle += uc;
        }
        else
        {
            qDebug() << "Replace" << uc.unicode() << "with space!";
            tmpTitle += QChar(' ');
        }
    }

    QTextCodec *pCodec = QTextCodec::codecForName("Shift_JIS");
    QTextEncoder *pEnc = pCodec->makeEncoder();
    if (pEnc)
    {
        if (static_cast<size_t>(pEnc->fromUnicode(tmpTitle).size()) != getHalfWidthTitleLength(title))
        {
            qDebug() << "Encoded size doesn't match -> fallback!";
            tmpTitle = title;
        }
        delete pEnc;
    }
    return tmpTitle;
}

QStringList unicodeSplit(const QString &src)
{
    QStringList ret = src.split(QString());
    ret.removeFirst();
    ret.removeLast();
    return ret;
}
