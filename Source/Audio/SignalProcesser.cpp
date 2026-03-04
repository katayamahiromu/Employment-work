#include"SignalProcessor.h"
#include"SignalMixer.h"
#include"WaveShaper.h"
#include"Oscillator.h"
#include <algorithm>

#define Mixer SignalMixer::instance()
#define Shaper WaveShaper::instance()
#define Oscillator Oscillator::instance()

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
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = samplingRate;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    data = std::make_shared<std::vector<UINT8>>();
}

SignalProcesser::~SignalProcesser()
{

}

void SignalProcesser::addWave(const std::vector<uint8_t>& data, float frequency, float gain)
{
    // バイト数が奇数なら無視する
    // 最後のバイトを切る
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

    float ms = 5.0f;
    Shaper->crossFade(wave.samples,ms);
    waveArray.emplace_back(wave);
}

void SignalProcesser::addWave(const std::vector<uint8_t>& data, float frequency, float gain, int index)
{
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

    float ms = 5.0f;
    Shaper->crossFade(wave.samples, ms);
    if (waveArray.size() <= index)waveArray.resize(index + 1);
    waveArray[index] = wave;
}

waveData SignalProcesser::createData(const std::vector<uint8_t>& data)
{
    size_t byteCount = data.size() & ~static_cast<size_t>(1);
    size_t samplesCount = byteCount / 2;

    waveData wave;
    wave.gain = 1.0f;
    wave.frequency = SamplingRate;
    wave.samples.resize(samplesCount);

    for (size_t i = 0; i < samplesCount; ++i)
    {
        uint16_t lo = static_cast<uint8_t>(data[i * 2]);
        uint16_t hi = static_cast<uint8_t>(data[i * 2 + 1]);
        uint16_t u = static_cast<uint16_t>(lo | (hi << 8));
        wave.samples[i] = static_cast<int16_t>(u);
    }

    return wave;
}

waveData SignalProcesser::createData(const std::vector<uint8_t>& data, float frequency,float phase)
{
    size_t byteCount = data.size() & ~static_cast<size_t>(1);
    size_t samplesCount = byteCount / 2;

    waveData wave;
    wave.gain = 1.0f;
    wave.frequency = frequency;
    wave.phase = phase;
    wave.samples.resize(samplesCount);

    for (size_t i = 0; i < samplesCount; ++i)
    {
        uint16_t lo = static_cast<uint8_t>(data[i * 2]);
        uint16_t hi = static_cast<uint8_t>(data[i * 2 + 1]);
        uint16_t u = static_cast<uint16_t>(lo | (hi << 8));
        wave.samples[i] = static_cast<int16_t>(u);
    }

    return wave;
}

void SignalProcesser::trySingleWave(int num)
{
    // 範囲チェック
    if (num < 0 || num >= static_cast<int>(waveArray.size()))
        return;

    const auto& wave = waveArray[num];

    // サンプル数がゼロなら何もしない
    if (wave.samples.empty()) {
        data.get()->clear();
        return;
    }

    // バイト数計算
    const size_t numSamples = wave.samples.size();
    const size_t requiredBytes = numSamples * sizeof(int16_t);

    data.get()->resize(requiredBytes);

    // コピー（バイト単位）
    std::memcpy(
        data.get()->data(),
        reinterpret_cast<const uint8_t*>(wave.samples.data()),
        requiredBytes
    );
}

void SignalProcesser::Mix()
{
    auto tmp = Mixer->mix(waveArray);
    data->swap(tmp);
}

void SignalProcesser::applyFM(int carriarIndex, int modIndex,float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    auto tmp = Mixer->frequencyModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
    data->swap(tmp);
}

void SignalProcesser::applyAM(int carriarIndex, int modIndex, float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    auto temp = Mixer->amplitudeModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
    data->swap(temp);
}

void SignalProcesser::applyRM(int carriarIndex, int modIndex, float modulationDepth, float gain)
{
    //配列外チェック
    if (!checkIndexes(carriarIndex, modIndex, waveArray.size()))return;
    auto tmp = Mixer->ringModulation(waveArray.at(carriarIndex), waveArray.at(modIndex), modulationDepth, gain);
    data->swap(tmp);
}

void SignalProcesser::decayWave(int index, float decay)
{
    if (!checkIndex(index, size()))return;
    addWave(Shaper->Decay(waveArray.at(index), decay));
}

void SignalProcesser::LowPass(int index, float cutoff, float depth)
{
    if (!checkIndex(index, size()))return;
    addWave(Shaper->LowPass(waveArray.at(index), cutoff, depth));
}

void SignalProcesser::BandPass(int index, float lowCut, float highCut)
{
    if (!checkIndex(index, size()))return;
    addWave(Shaper->BandPass(waveArray.at(index), lowCut, highCut));
}