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
#include <QFileInfo>
#include <QApplication>
#include "cdrutil.h"
#include "helpers.h"
#include <cmath>
#include <QXmlStreamReader>

CDRUtil::CDRUtil(QObject *parent)
    : CCliProcess(parent)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CDRUtil::finish);
}

int CDRUtil::start()
{
    QStringList params;
    mLog.clear();

    params << "cdtext";

    qInfo() << DRUTIL_CLI << params;
    run(DRUTIL_CLI, params);
    return 0;
}

void CDRUtil::finish(int exitCode, ExitStatus exitStatus)
{
    CDTextData cdtdata;
    if ((exitCode == 0) && (exitStatus == ExitStatus::NormalExit))
    {
        mLog += QString::fromUtf8(readAllStandardOutput());

        // find xml start- and end marker
        int start = mLog.indexOf("<?xml");
        int end   = mLog.lastIndexOf("</plist>");

        if ((start > -1) && (end > -1))
        {
            QString content = mLog.mid(start, 8 + end - start);
            parseXml(content, cdtdata);
        }
    }

    if (!mLog.isEmpty())
    {
        qDebug() << static_cast<const char*>(mLog.toUtf8());
    }

    emit fileDone(cdtdata);
}

//--------------------------------------------------------------------------
//! @brief      parse output of drutil (xml)
//!
//! @param[in]  xmlData    xml data string
//! @param[in]  cdtdata    buffer for CD-TEXT data
//!
//! @return 0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CDRUtil::parseXml(const QString &xmlData, CDTextData &cdtdata)
{
    SCDText cdt;
    QXmlStreamReader xml;
    xml.addData(xmlData);
    enum Cap {
        ARTIST,
        TITLE,
        NONE
    } capNext = NONE;

    cdtdata.clear();

    while(!xml.atEnd() && !xml.hasError())
    {
        if (xml.isStartElement() && (xml.name() == "key"))
        {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
                if (xml.text() == "DRCDTextPerformerKey")
                {
                    capNext = Cap::ARTIST;
                }
                else if (xml.text() == "DRCDTextTitleKey")
                {
                    capNext = Cap::TITLE;
                }
                else
                {
                    capNext = Cap::NONE;
                }
            }
        }
        else if ((xml.isStartElement() && (xml.name() == "string")) && (capNext != Cap::NONE))
        {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
                switch(capNext)
                {
                case Cap::ARTIST:
                    cdt.mArtist = xml.text().toString();
                    break;
                case Cap::TITLE:
                    cdt.mTitle = xml.text().toString();
                    break;
                default:
                    break;
                }
            }
            capNext = Cap::NONE;
        }
        else if (xml.isEndElement() && (xml.name() == "dict"))
        {
            if (!cdt.mArtist.isEmpty() || !cdt.mTitle.isEmpty())
            {
                cdtdata.append(cdt);
                cdt = {"", ""};
            }
        }
    }

    return cdtdata.isEmpty() ? -1 : 0;
}
