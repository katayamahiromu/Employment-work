#include"SignalProcessor.h"
#include"SignalMixer.h"
#include"WaveShaper.h"

#define Mixer SignalMixer::instance()
#define Shaper WaveShaper::instance()

static inline bool checkIndex(int index,size_t size)
{
    return (index >= 0) && (static_cast<size_t>(index) < size);
}

static inline bool checkIndexes(int index1, int index2, size_t size)
{
   return (checkIndex(index1, size) && checkIndex(index2, size));
}

SignalProcesser::SignalProcesser()
{
    samplingRate = 44100;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = samplingRate;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
}

SignalProcesser::~SignalProcesser()
{

}

void SignalProcesser::addWave(const std::vector<uint8_t>& data, float frequency, float gain)
{
    // バイト数が奇数なら無視する最後のバイトを切る
    size_t byteCount = data.size() & ~static_cast<size_t>(1);
    size_t samplesCount = byteCount / 2;

    waveData wave;
    wave.gain = gain;
    wave.frequency = frequency;
    wave.samples.resize(samplesCount);

    for (size_t i = 0; i < samplesCount; ++i)
    {
        uint16_t lo = static_cast<uint8_t>(data[i * 2]);
        uint16_t hi = static_cast<uint8_t>(data[i * 2 + 1]);
        uint16_t u = static_cast<uint16_t>(lo | (hi << 8));
        wave.samples[i] = static_cast<int16_t>(u);
    }

    waveArray.emplace_back(wave);
}

void SignalProcesser::trySingleWave(int num)
{
    //範囲チェック
    if (num < 0 || num >= static_cast<int>(waveArray.size())) return;

    auto wave = waveArray.at(num);
    int numSamples = static_cast<int>(wave.samples.size());

    data.resize(numSamples * 2);
    for (size_t i = 0; i < numSamples; ++i) {
        int16_t s = static_cast<int16_t>(wave.samples[i]);
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }
}

void SignalProcesser::Mix()
{
    data = Mixer->mix(waveArray);
}

void SignalProcesser::applyFM(int carriarIndex, int modIndex,float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    data = Mixer->frequencyModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
}

void SignalProcesser::applyAM(int carriarIndex, int modIndex, float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    data = Mixer->amplitudeModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
}

void SignalProcesser::applyRM(int carriarIndex, int modIndex, float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    data = Mixer->ringModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
}

void SignalProcesser::decayWave(int index, float decay)
{
    if (!checkIndex(index, size()))return;
    addWave(Shaper->Decay(waveArray.at(index), decay));
}