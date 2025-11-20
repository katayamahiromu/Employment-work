#include"SoundGenerator.h"
#include<random>
#include <algorithm>
#include<math.h>


static inline void pushInt16LE(std::vector<uint8_t>& out, int16_t v)
{
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
}

std::vector<uint8_t> SoundGenerator::sinWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
 
    std::vector<uint8_t>samples(numSamples * 2);

    for (int i = 0; i < numSamples; ++i) 
    {
        double t = static_cast<double>(i) / sampleRate;
        double v = 30000.0 * std::sin(2.0 * DirectX::XM_PI * frequency * t); // -30000..30000
        int16_t s = static_cast<int16_t>(std::lround(v));
        pushInt16LE(samples, s);
    }
    return samples;
}

std::vector<uint8_t> SoundGenerator::sawtoothWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    for (int i = 0; i < numSamples; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double period = 1.0 / frequency;
        double value = 2.0 * (t / period - std::floor(t / period + 0.5)); // -1..1
        double v = 30000.0 * value;
        int16_t s = static_cast<int16_t>(std::lround(v));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> SoundGenerator::triangleWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    for (int i = 0; i < numSamples; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double period = 1.0 / frequency;
        double value = 2.0 * std::fabs(2.0 * (t / period - std::floor(t / period + 0.5))) - 1.0; // -1..1
        double v = 30000.0 * value;
        int16_t s = static_cast<int16_t>(std::lround(v));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> SoundGenerator::squareWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    for (int i = 0; i < numSamples; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double period = 1.0 / frequency;
        double value = (std::fmod(t, period) < period / 2.0) ? 1.0 : -1.0;
        double v = 30000.0 * value;
        int16_t s = static_cast<int16_t>(std::lround(v));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> SoundGenerator::whiteNoise(float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(-30000, 30000);

    for (int i = 0; i < numSamples; ++i)
    {
        int16_t s = static_cast<int16_t>(distribution(generator));
        pushInt16LE(samples, s);
    }

    return samples;

}

std::vector<uint8_t> SoundGenerator::impactSound(float intensity, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    for (int i = 0; i < numSamples; ++i)
    {
        double t = static_cast<double>(i) / sampleRate;
        double envelope = std::exp(-5.0 * t); // 急速な減衰
        double freq = 800.0 + intensity * 1200.0;
        double value = envelope * std::sin(2.0 * DirectX::XM_PI * freq * t); // -1..1 (scaled by envelope)
        double v = 30000.0 * value;
        int16_t s = static_cast<int16_t>(std::lround(v));
        pushInt16LE(samples, s);
    }
    return samples;
}

float SoundGenerator::adsrEnvelope(float t, float duration,
    float attack, float decay,
    float sustainLevel, float release)
{
    if (t < attack) {
        return t / attack;
    }
    else if (t < attack + decay) {
        return 1.0f - (1.0f - sustainLevel) * ((t - attack) / decay);
    }
    else if (t < duration - release) {
        return sustainLevel;
    }
    else if (t < duration) {
        return sustainLevel * (1.0f - (t - (duration - release)) / release);
    }
    else {
        return 0.0f;
    }
}

SoundMixer::SoundMixer(int samplingRate):samplingRate(44100)
{
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = samplingRate;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
}

SoundMixer::~SoundMixer()
{

}

void SoundMixer::addWave(const std::vector<uint8_t>& data, float frequency, float gain)
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

void SoundMixer::erase(int num)
{
    waveArray.erase(waveArray.begin() + num);
}

void SoundMixer::mix()
{
    //配列がからのときは何もしない
    if (waveArray.empty())return;

    //サンプル数の取得
    size_t maxSamples = 0;
    for (const auto& w : waveArray) {
        maxSamples = max(maxSamples, w.samples.size());
    }
    if (maxSamples == 0) { data.clear(); return; }

    // 合成用バッファ
    std::vector<int> buf(maxSamples, 0);

    // 加算合成（gain を掛ける）
    for (const auto& w : waveArray) {
        float g = w.gain;
        for (size_t i = 0; i < w.samples.size(); ++i) {
            // 丸めは最後に行う。ここでは float 演算を int に加算するため一旦 float を int に変換
            buf[i] += static_cast<int>(std::lround(static_cast<float>(w.samples[i]) * g));
        }
    }

    // 最大振幅を求める（正規化用）
    int maxAmp = 0;
    for (auto v : buf) maxAmp = max(maxAmp, std::abs(v));

    // ゼロ除算回避
    float norm = (maxAmp > 0) ? (32767.0f / static_cast<float>(maxAmp)) : 1.0f;

    // 出力バイト列に格納（PCM16 LE）
    data.resize(maxSamples * 2);
    for (size_t i = 0; i < maxSamples; ++i) {
        float scaled = buf[i] * norm;
        // クリッピング
        if (scaled > 32767.0f) scaled = 32767.0f;
        if (scaled < -32768.0f) scaled = -32768.0f;
        int16_t s = static_cast<int16_t>(std::lround(scaled));
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }
}

void SoundMixer::allFM(float modulationDepth)
{
    if (waveArray.empty()) return;

    // 出力長は最大サンプル長
    size_t maxSamples = 0;
    for (const auto& w : waveArray) maxSamples = max(maxSamples, w.samples.size());
    if (maxSamples == 0) { data.clear(); return; }

    // 合成用バッファ
    std::vector<int> buf(maxSamples, 0);

    // 各キャリアごとに位相を持たせる（初期位相は 0）
    std::vector<double> phases(waveArray.size(), 0.0);
    // 各キャリアの基本位相増分
    std::vector<double> dphis(waveArray.size(), 0.0);
    for (size_t i = 0; i < waveArray.size(); ++i) {
        double f = static_cast<double>(waveArray[i].frequency);
        if (f > 0.0)
            dphis[i] = 2.0 * DirectX::XM_PI * f / static_cast<double>(samplingRate);
        else
            dphis[i] = 0.0;
    }

    // モジュレータ数（キャリアを除く固定値。0 にならないようにする）
    size_t modCountFixed = (waveArray.size() > 1) ? (waveArray.size() - 1) : 1;

    // 合成ループ
    for (size_t ci = 0; ci < waveArray.size(); ++ci)
    {
        const auto& carrier = waveArray[ci];
        if (carrier.frequency <= 0.0) continue; // 周波数情報が必要

        double phaseC = phases[ci];
        double dphiC = dphis[ci];

        for (size_t n = 0; n < maxSamples; ++n)
        {
            // モジュレータ合成（ループ再生で参照）
            double modSum = 0.0;
            for (size_t mi = 0; mi < waveArray.size(); ++mi) {
                if (mi == ci) continue;
                const auto& mod = waveArray[mi];
                if (mod.samples.empty()) continue;
                // ループ再生インデックス
                size_t idx = n % mod.samples.size();
                double sample = static_cast<double>(mod.samples[idx]) / 32767.0;
                // モジュレータのゲインも考慮
                modSum += sample * static_cast<double>(mod.gain);
            }
            double modSample = modSum / static_cast<double>(modCountFixed);

            // 位相増分にモジュレーションを加える
            double dphi = dphiC + static_cast<double>(modulationDepth) * modSample;
            phaseC += dphi;

            // 出力サンプル（キャリアのゲインを掛ける）
            double sampleVal = std::sin(phaseC) * 32767.0 * static_cast<double>(carrier.gain);
            buf[n] += static_cast<int>(std::lround(sampleVal));

            // 位相のラップ（数値安定化）
            if (phaseC > 1e9 || phaseC < -1e9) phaseC = std::fmod(phaseC, 2.0 * DirectX::XM_PI);
        }

        // 保存しておく（将来の呼び出しで位相連続性を保ちたい場合）
        phases[ci] = phaseC;
    }

    // 正規化（既存 mix() と同様）
    int maxAmp = 0;
    for (auto v : buf) maxAmp = max(maxAmp, std::abs(v));
    float norm = (maxAmp > 0) ? (32767.0f / static_cast<float>(maxAmp)) : 1.0f;

    data.resize(maxSamples * 2);
    for (size_t i = 0; i < maxSamples; ++i) {
        float scaled = buf[i] * norm;
        if (scaled > 32767.0f) scaled = 32767.0f;
        if (scaled < -32768.0f) scaled = -32768.0f;
        int16_t s = static_cast<int16_t>(std::lround(scaled));
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }

}

void SoundMixer::selectData(int num)
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

void SoundMixer::applyFM(int carrierIndex, int modIndex, float modulationDepth,float gain)
{
    // 範囲チェック
    if (carrierIndex < 0 || carrierIndex >= static_cast<int>(waveArray.size())) return;
    if (modIndex < 0 || modIndex >= static_cast<int>(waveArray.size())) return;

    const auto& carrier = waveArray[carrierIndex].samples;
    const auto& modulator = waveArray[modIndex].samples;
    float carrierFreq = waveArray[carrierIndex].frequency;
    float modFreq = waveArray[modIndex].frequency;

    // 出力長は最大長に合わせる（短い方はループ参照）
    size_t csize = carrier.size();
    size_t msize = modulator.size();
    size_t numSamples = max(csize, msize);
    if (numSamples == 0) return;

    // 合成用バッファ（int）
    std::vector<int> buf(numSamples, 0);

    // キャリア位相と基本位相増分
    double phaseC = 0.0;
    double dphiC = (carrierFreq > 0.0f) ? 2.0 * DirectX::XM_PI * static_cast<double>(carrierFreq) / samplingRate : 0.0;
    // モジュレータ位相増分（位相ベースのモジュレータを使う場合に備える）
    double phaseM = 0.0;
    double dphiM = (modFreq > 0.0f) ? 2.0 * DirectX::XM_PI * static_cast<double>(modFreq) / samplingRate : 0.0;

    // ループ参照でモジュレータを使う（短い波形が途中で消えない）
    for (size_t n = 0; n < numSamples; ++n)
    {
        // モジュレータ値 (-1.0〜+1.0)
        double mod = 0.0;
        if (msize > 0) {
            size_t idx = n % msize;
            mod = (double)modulator[idx] / 32767.0 * waveArray[modIndex].gain;
        }

        // FM: 位相に直接 I・sin(...) を加算する
        phaseC += dphiC;               // キャリアの基本位相
        double fmPhase = phaseC + modulationDepth * sin(phaseM);

        // 出力
        double out = sin(fmPhase) * 32767.0 * waveArray[carrierIndex].gain * gain;
        buf[n] = (int)std::lround(out);

        // モジュレータの位相進行
        phaseM += dphiM;

        // 位相ラップ
        if (phaseC > 2.0 * DirectX::XM_PI) phaseC -= 2.0 * DirectX::XM_PI;
        if (phaseM > 2.0 * DirectX::XM_PI) phaseM -= 2.0 * DirectX::XM_PI;
    }

    // 合成結果を一度だけ正規化（mix() と同様）
    int maxAmp = 0;
    for (auto v : buf) maxAmp = max(maxAmp, std::abs(v));
    float norm = (maxAmp > 0) ? (32767.0f / static_cast<float>(maxAmp)) : 1.0f;

    // data に PCM16LE 格納
    data.resize(numSamples * 2);
    for (size_t i = 0; i < numSamples; ++i) {
        float scaled = buf[i] * norm;
        if (scaled > 32767.0f) scaled = 32767.0f;
        if (scaled < -32768.0f) scaled = -32768.0f;
        int16_t s = static_cast<int16_t>(std::lround(scaled));
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }
}