#include"Oscillator.h"
#include<random>
#include <algorithm>
#include<math.h>
#include"SoundHealper.h"

std::vector<uint8_t> Oscillator::sinWave(float frequency, float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::sawtoothWave(float frequency, float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::triangleWave(float frequency, float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::squareWave(float frequency, float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::whiteNoise(float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::impactSound(float intensity, float durationSeconds, int sampleRate)
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

std::vector<uint8_t> Oscillator::impactModes(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain)
{
    int numSamples = static_cast<int>(durationSeconds * SamplingRate);
    if (numSamples <= 0) return std::vector<uint8_t>{};

    // 出力バッファ（バイト列）。pushInt16LE で追加するので reserve しておく
    std::vector<uint8_t> samples;
    samples.reserve(static_cast<size_t>(numSamples) * 2);

    // 各モードの事前計算（開始サンプル、角周波数、位相インクリメント）
    struct ModePrep {
        int startSample;
        double dphi;
        double phase0;
        double tau;
        const ModalMode* mode;
    };

    std::vector<ModePrep> prep;
    prep.reserve(modeCount);
    for (size_t i = 0; i < modeCount; ++i) {
        const ModalMode& m = modes[i];
        ModePrep p;
        p.startSample = static_cast<int>(std::round(m.startSec * static_cast<float>(SamplingRate)));
        p.dphi = DirectX::XM_2PI * static_cast<double>(m.frequency) / static_cast<double>(SamplingRate);
        p.phase0 = static_cast<double>(m.phase);
        p.tau = max(1e-6, static_cast<double>(m.decayTime));
        p.mode = &m;
        prep.push_back(p);
    }

    // サンプルごとに全モードを合成
    for (int n = 0; n < numSamples; ++n)
    {
        double acc = 0.0;

        for (const auto& p : prep)
        {
            const ModalMode& m = *p.mode;
            if (m.amplitude <= 0.0f || m.gain == 0.0f) continue;
            if (n < p.startSample) continue;

            int idx = n - p.startSample;
            double t = static_cast<double>(idx) / static_cast<double>(SamplingRate);

            // 指数減衰（1/e 時定数 tau）
            double env = static_cast<double>(m.amplitude) * std::exp(-t / p.tau);
            if (env * std::abs(m.gain) < 1e-8) continue; // 十分小さければ無視

            // 位相（初期位相 + インクリメント * サンプル数）
            double phase = p.phase0 + p.dphi * static_cast<double>(idx);
            // サイン波
            double s = env * std::sin(phase);

            // モードゲインを適用して 16bit スケールに寄せる（マスターゲインは最後にまとめても可）
            acc += s * static_cast<double>(m.gain) * 32767.0;
        }

        // マスターゲイン適用、クリップして int16 に
        double v = acc * static_cast<double>(masterGain);
        if (v > 32767.0) v = 32767.0;
        if (v < -32768.0) v = -32768.0;
        int16_t outSample = static_cast<int16_t>(std::lround(v));

        // PCM16LE にプッシュ（ヘルパー関数を使用）
        pushInt16LE(samples, outSample);
    }

    return samples;
}