/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 08.02.2010 / 10:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "ctranslit.h"
#include "mdtitle.h"

/********************************************************************\
 *  This translit stuff only works, if you save this file in        *
 *  UTF-8 format!                                                   *
 *                                                                  *
 *    !!! Using another coding will break the functionality !!!     *
 *                                                                  *
 *  Make sure that your project uses UTF-8 encoding for text!       *
 *  To do this, check out the project settings!                     *
\********************************************************************/

/* -----------------------------------------------------------------\
|  Method: CTranslit / constructor
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: init translation tables
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CTranslit::CTranslit()
{
   // fill map (cyrillic -> latin) ...
   mCyr2Lat[QString::fromUtf8("а")] = "a";
   mCyr2Lat[QString::fromUtf8("б")] = "b";
   mCyr2Lat[QString::fromUtf8("в")] = "v";
   mCyr2Lat[QString::fromUtf8("г")] = "g";
   mCyr2Lat[QString::fromUtf8("д")] = "d";
   mCyr2Lat[QString::fromUtf8("е")] = "e";
   mCyr2Lat[QString::fromUtf8("ё")] = "e";
   mCyr2Lat[QString::fromUtf8("ж")] = "zh";
   mCyr2Lat[QString::fromUtf8("з")] = "z";
   mCyr2Lat[QString::fromUtf8("и")] = "i";
   mCyr2Lat[QString::fromUtf8("й")] = "j";
   mCyr2Lat[QString::fromUtf8("к")] = "k";
   mCyr2Lat[QString::fromUtf8("л")] = "l";
   mCyr2Lat[QString::fromUtf8("м")] = "m";
   mCyr2Lat[QString::fromUtf8("н")] = "n";
   mCyr2Lat[QString::fromUtf8("о")] = "o";
   mCyr2Lat[QString::fromUtf8("п")] = "p";
   mCyr2Lat[QString::fromUtf8("р")] = "r";
   mCyr2Lat[QString::fromUtf8("с")] = "s";
   mCyr2Lat[QString::fromUtf8("т")] = "t";
   mCyr2Lat[QString::fromUtf8("у")] = "u";
   mCyr2Lat[QString::fromUtf8("ф")] = "f";
   mCyr2Lat[QString::fromUtf8("х")] = "h";
   mCyr2Lat[QString::fromUtf8("ц")] = "c";
   mCyr2Lat[QString::fromUtf8("ш")] = "sh";
   mCyr2Lat[QString::fromUtf8("щ")] = "sch";
   mCyr2Lat[QString::fromUtf8("ч")] = "ch";
   mCyr2Lat[QString::fromUtf8("ы")] = "y";
   mCyr2Lat[QString::fromUtf8("ъ")] = "´";
   mCyr2Lat[QString::fromUtf8("ь")] = "'";
   mCyr2Lat[QString::fromUtf8("э")] = "e'";
   mCyr2Lat[QString::fromUtf8("ю")] = "yu";
   mCyr2Lat[QString::fromUtf8("я")] = "ya";

   // upper case
   QMap<QString, QString> tmpMap;
   for (auto key : mCyr2Lat.keys())
   {
       QString val = mCyr2Lat.value(key);
       val[0] = val[0].toUpper();
       key[0] = key[0].toUpper();

       tmpMap.insert(key, val);
   }

   for (const auto& k : tmpMap.keys())
   {
       mCyr2Lat.insert(k, tmpMap.value(k));
   }
}

/* -----------------------------------------------------------------\
|  Method: CyrToLat
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: make translit cyrillic --> latin
|
|  Parameters: ref. to cyrillic string
|
|  Returns: latin string
\----------------------------------------------------------------- */
QString CTranslit::CyrToLat(const QString &str, bool fileName)
{
    QString sOut = str;

    auto uniCodeToks = unicodeSplit(str);
    for (const auto& c : uniCodeToks)
    {
        if (mCyr2Lat.keys().contains(c))
        {
            sOut.replace(c, mCyr2Lat.value(c));
        }
    }

    if (fileName)
    {
        sOut.replace(" ", "_");
        sOut.replace("'", "");
        sOut.replace("´", "");
        sOut = sOut.toUpper();
    }

    return sOut;
}

/* -----------------------------------------------------------------\
|  Method: LatToCyr
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: make translit latin --> cyrillic
|
|  Parameters: ref. to latin string
|
|  Returns: cyrillic string
\----------------------------------------------------------------- */
QString CTranslit::LatToCyr(const QString &str, bool fileName)
{
    QString sOut = str;

    auto toks = mCyr2Lat.values();

    for (const auto& tok : toks)
    {
        if (sOut.contains(tok))
        {
            sOut.replace(tok, mCyr2Lat.key(tok));
        }
    }

    if (fileName)
    {
        sOut.replace(" ", "_");
        sOut.replace("'", "");
        sOut.replace("´", "");
        sOut = sOut.toUpper();
    }

    return sOut;
}
