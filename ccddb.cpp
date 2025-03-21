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
#include "ccddb.h"
#include <QNetworkReply>
#include <QRegExp>
#include "defines.h"

CCDDB::CCDDB(QObject *parent) : QObject(parent)
{
    mpNetwork = new QNetworkAccessManager(parent);

    if (mpNetwork != nullptr)
    {
        connect(mpNetwork, &QNetworkAccessManager::finished, this, &CCDDB::catchResponse);
    }
}

//--------------------------------------------------------------------------
//! @brief      Gets the entries.
//!
//! @param[in]  queryPart  The query part
//! @param[in]  tracks     The prepared audio tracks
//!
//! @return     0 -> ok; -1 -> error
//--------------------------------------------------------------------------
int CCDDB::getEntries(const QString& queryPart, c2n::AudioTracks& tracks)
{
    mAudioTracks = tracks;
    QUrl reqUrl(QString(CDDB_SERVER)
             + "/~cddb/cddb.cgi?cmd=cddb+query+" + queryPart
             + "&hello=me@you.org+localhost+MyRipper+0.1.0&proto=6");

    qDebug() << QSslSocket::supportsSsl()
             << QSslSocket::sslLibraryBuildVersionString()
             << QSslSocket::sslLibraryVersionString();

    qInfo() << "Request entries: " << reqUrl;


    QNetworkRequest req(reqUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=utf-8");
    req.setAttribute(QNetworkRequest::User, static_cast<int>(REQ_WHAT::REQ_ENTRIES));

    return (mpNetwork->get(req) == nullptr) ? -1 : 0;
}

int CCDDB::getEntry(const QString &queryPart)
{
    QUrl reqUrl(QString(CDDB_SERVER)
             + "/~cddb/cddb.cgi?cmd=cddb+read+" + queryPart
             + "&hello=me@you.org+localhost+MyRipper+0.1.0&proto=6");

    qInfo() << "Request matching entry: " << reqUrl;

    QNetworkRequest req(reqUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=utf-8");
    req.setAttribute(QNetworkRequest::User, static_cast<int>(REQ_WHAT::REQ_MATCH));

    return (mpNetwork->get(req) == nullptr) ? -1 : 0;
}

int CCDDB::catchResponse(QNetworkReply *reply)
{
    int ret;

    if (reply->error() == QNetworkReply::NoError)
    {
        REQ_WHAT what   = static_cast<REQ_WHAT>(reply->request().attribute(QNetworkRequest::User).toInt());
        QString  sReply = QString::fromUtf8(reply->readAll());

        ret = parseReply(what, sReply);
    }
    else
    {
        qCritical() << reply->errorString();
        emit match(mAudioTracks);
        ret = -1;
    }

    reply->deleteLater();

    return ret;
}

int CCDDB::parseReply(CCDDB::REQ_WHAT type, QString reply)
{
    // remove CR
    reply.replace(QChar('\r'), "");
    QRegExp rxEntries("^([^ ]+) ([^ ]+) (.*)$");
    QRegExp rxMatch("^[DT]{1,1}TITLE[^=]*=[ ]*(.*)$");
    QRegExp rxYear("^DYEAR[^=]*=[ ]*(.*)$");
    int year = -1;
    QStringList results;

    // split in lines
    QStringList content = reply.split('\n');
    int         lineNo  = 0;
    int         code    = 0;
    for (auto tok : content)
    {
        if (type == REQ_WHAT::REQ_ENTRIES)
        {
            if (lineNo == 0)
            {
                // get code
                code = tok.mid(0, 3).toInt();

                // cut out code
                tok  = tok.mid(4);
            }

            if (tok == ".")
            {
                break;
            }

            if ((code == 200) || (((code == 210) || (code == 211)) && (lineNo > 0)))
            {
                if (rxEntries.indexIn(tok) > -1)
                {
                    results.append(QString("%1+%2\t%3").arg(rxEntries.cap(1))
                                   .arg(rxEntries.cap(2)).arg(rxEntries.cap(3)));
                }
            }
        }
        else if (type == REQ_WHAT::REQ_MATCH)
        {
            if (rxMatch.indexIn(tok) > -1)
            {
                tok = rxMatch.cap(1);
                tok.replace('/', '-');
                results.append(tok);
            }
            else if (rxYear.indexIn(tok) > -1)
            {
                // capture year
                year = rxYear.cap(1).toInt();
                qInfo() << "Found year" << year << "in CDDB response!";
            }
        }
        lineNo ++;
    }

    if (type == REQ_WHAT::REQ_ENTRIES)
    {
        if (results.isEmpty())
        {
            // no match
            if (!mAudioTracks.isEmpty())
            {
                mAudioTracks[0].mTitle = tr("No CDDB Entry found!");
            }
            emit match(mAudioTracks);
        }
        else if (results.size() == 1)
        {
            // only one entry -> get it
            QStringList sl = results.at(0).split('\t');
            getEntry(sl.at(0));
        }
        else
        {
            emit entries(results);
        }
        return 0;
    }
    else if (type == REQ_WHAT::REQ_MATCH)
    {
        QDateTime tstamp;

        if (year > 0)
        {
            tstamp.setDate(QDate(year, 11, 11));
            tstamp.setTime(QTime(11, 11, 11));
        }

        for(int s = 0; s < mAudioTracks.size(); s++)
        {
            if (s < results.size())
            {
                mAudioTracks[s].mTitle = results[s];
                if (tstamp.isValid())
                {
                    mAudioTracks[s].mTStamp = tstamp;
                }
            }
        }

        emit match(mAudioTracks);
        return 0;
    }

    return -1;
}
