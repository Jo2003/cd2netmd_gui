#include <QRegExp>
#include "ccliprocess.h"

CCliProcess::CCliProcess(QObject *parent)
    :QProcess(parent)
{
    QProcess::setProcessChannelMode(ProcessChannelMode::MergedChannels);
    QProcess::setReadChannel(ProcessChannel::StandardOutput);
    connect(this, &QProcess::readyReadStandardOutput, this, &CCliProcess::extractPercent);
}

void CCliProcess::run(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
    mLog.clear();
    QProcess::start(program, arguments, mode);
    waitForStarted();
}

bool CCliProcess::busy() const
{
    return state() != QProcess::NotRunning;
}

void CCliProcess::extractPercent()
{
    mLog += QString::fromUtf8(readAllStandardOutput());
    int pos;

    if ((pos = mLog.lastIndexOf(QChar('%'))) > 0)
    {
        int startPos = mLog.lastIndexOf(QRegExp("[^0-9]+"), pos - 1);

        if (startPos > -1)
        {
            bool ok;
            int length  = (pos - 1) - startPos;
            int percent = mLog.mid(startPos + 1, length).toInt(&ok);

            if (ok)
            {
                emit progress(percent);
            }
        }
    }
}
