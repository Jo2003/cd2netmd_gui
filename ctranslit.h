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
#pragma once
#include <QString>
#include <QMap>

/********************************************************************\
|  Class: CTranslit
|  Date:  08.02.2010 / 11:00:28
|  Author: Jo2003
|  Description: class to make translit between latin and cyrillic
|
\********************************************************************/
class CTranslit
{
public:
    CTranslit();

    QString CyrToLat (const QString &str, bool fileName = true);
    QString LatToCyr (const QString &str, bool fileName = true);

private:
    QMap<QString, QString> mCyr2Lat;
};

