#pragma once
#include <QObject>
#include "utils.h"
#include "defines.h"

class CWaveSplitter : public QObject
{
    Q_OBJECT

    // data needed for 1 sec in 44100KHz, 16bit, stereo
    static constexpr uint32_t WAVE_BLOCK_SIZE = 44100 * 2 * 2;
    static constexpr uint32_t WAVE_FRAME_SIZE = 2048;

public:
    explicit CWaveSplitter(QObject *parent = nullptr);
    void splitWaves(const c2n::TransferQueue& queue);
    int checkWaveFile(QFile& fWave, size_t& waveDataSize);
};
