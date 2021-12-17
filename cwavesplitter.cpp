#include "cwavesplitter.h"
#include <QDir>
#include <cstring>
#include <stdexcept>
#include <QtEndian>

CWaveSplitter::CWaveSplitter(QObject *parent)
    :QObject(parent)
{
}

void CWaveSplitter::splitWaves(const c2n::TransferQueue &queue)
{
    QFile sourceWave(queue[0].mpFile->fileName());
    if (sourceWave.open(QIODevice::ReadOnly))
    {
        size_t wholeSz = 0, read = 0;
        if (checkWaveFile(sourceWave, wholeSz) == 0)
        {
            int trackCount = 0;
            for (const auto& t : queue)
            {
                QString fileName = t.mpFile->fileName();

                trackCount ++;

                if (trackCount == 1)
                {
                    fileName += "buf";
                }

                QFile track(fileName);
                if (track.open(QIODevice::Truncate | QIODevice::WriteOnly))
                {
                    size_t sz = t.mLength * WAVE_BLOCK_SIZE;
                    if (sz % WAVE_FRAME_SIZE)
                    {
                        sz += WAVE_FRAME_SIZE - (sz % WAVE_FRAME_SIZE);
                    }

                    if (trackCount == queue.size())
                    {
                        writeWaveHeader(track, wholeSz - read);
                        track.write(sourceWave.readAll());
                        qDebug("Wrote last file %s with %llu bytes",
                               static_cast<const char*>(track.fileName().toUtf8()), wholeSz - read);
                    }
                    else
                    {
                        writeWaveHeader(track, sz);
                        track.write(sourceWave.read(sz));
                        read += sz;
                        qDebug("Wrote file %s with %llu bytes",
                               static_cast<const char*>(track.fileName().toUtf8()), sz);
                    }
                    track.close();
                }
            }

            sourceWave.close();

            // copy first track
            QFile f1(queue.at(0).mpFile->fileName());
            QFile f2(queue.at(0).mpFile->fileName() + "buf");

            if (f1.open(QIODevice::Truncate | QIODevice::WriteOnly) && f2.open(QIODevice::ReadOnly))
            {
                f1.write(f2.readAll());
                f1.close();
                f2.close();
                f2.remove();
            }
        }
    }
}

int CWaveSplitter::checkWaveFile(QFile &fWave, size_t &waveDataSize)
{
    int ret = 0;
    try
    {
        // read complete WAV header as written by our Jack the Ripper
        QByteArray data = fWave.read(44);
        if (strncmp(static_cast<const char*>(data), "RIFF", 4) != 0)
        {
            throw std::runtime_error(R"("RIFF" marker not detected!)");
        }
        if (strncmp(static_cast<const char*>(data) + 8, "WAVEfmt ", 8) != 0)
        {
            throw std::runtime_error(R"("WAVEfmt " marker not detected!)");
        }
        if (strncmp(static_cast<const char*>(data) + 36, "data", 4) != 0)
        {
            throw std::runtime_error(R"("data" marker not detected!)");
        }
        waveDataSize = qFromLittleEndian<uint32_t>(static_cast<const char*>(data) + 40);
    }
    catch (const std::exception& e)
    {
        qDebug("%s", e.what());
        ret = -1;
    }
    return ret;
}
