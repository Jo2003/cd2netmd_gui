#pragma once
#include <QObject>
#include <QProcess>

class CCliProcess : public QProcess
{
    Q_OBJECT
public:
    CCliProcess(QObject* parent = nullptr);
    void run(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode = ReadWrite);
    bool busy() const;

private slots:
    void extractPercent();

signals:
    void progress(int);

protected:
    QString mLog;
};

