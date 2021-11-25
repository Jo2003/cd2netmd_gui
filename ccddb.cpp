#include "ccddb.h"
#include <QNetworkReply>
#include <QRegExp>

CCDDB::CCDDB(QObject *parent) : QObject(parent)
{
    mpNetwork = new QNetworkAccessManager(parent);

    if (mpNetwork != nullptr)
    {
        connect(mpNetwork, &QNetworkAccessManager::finished, this, &CCDDB::catchResponse);
    }
}

int CCDDB::getEntries(const QString &queryPart)
{
    QUrl reqUrl(QString(CDDB_SERVER)
             + "/~cddb/cddb.cgi?cmd=cddb+query+" + queryPart
             + "&hello=me@you.org+localhost+MyRipper+0.1.0&proto=6");

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
        }
        lineNo ++;
    }

    if (type == REQ_WHAT::REQ_ENTRIES)
    {
        if (results.isEmpty())
        {
            // no match
            emit match(results);
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
        emit match(results);
        return 0;
    }

    return -1;
}
