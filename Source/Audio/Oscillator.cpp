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

    std::vector<uint8_t> samples;
    samples.reserve(static_cast<size_t>(numSamples) * 2);

    struct ModePrep {
        int startSample;
        double dphi;
        double phase0;
        double tau;
        const ModalMode* mode;
    };

    std::vector<ModePrep> prep;
    prep.reserve(modeCount);

    // --- モードごとの事前計算 ---
    for (size_t i = 0; i < modeCount; ++i) {
        const ModalMode& m = modes[i];
        ModePrep p;
        p.startSample = static_cast<int>(std::round(m.startSec * SamplingRate));

        // 周波数に inharmonicity を反映
        double freq = static_cast<double>(m.frequency) * (1.0 + static_cast<double>(m.inharmonicity));
        p.dphi = DirectX::XM_2PI * freq / SamplingRate;
        p.phase0 = static_cast<double>(m.phase);

        // decayTime にランダム揺らぎ
        double decayRand = 1.0;
        if (m.randomDecay > 0.0f) {
            decayRand += ((double)rand() / RAND_MAX * 2.0 - 1.0) * m.randomDecay;
        }
        p.tau = max(1e-6, static_cast<double>(m.decayTime) * decayRand);

        p.mode = &m;
        prep.push_back(p);
    }

    // フィルタ用の状態変数
    double prevSample = 0.0;

    // --- サンプル生成ループ ---
    for (int n = 0; n < numSamples; ++n) {
        double acc = 0.0;

        // 各モードの合成
        for (const auto& p : prep) {
            const ModalMode& m = *p.mode;
            if (m.amplitude <= 0.0f || m.gain == 0.0f) continue;
            if (n < p.startSample) continue;

            int idx = n - p.startSample;
            double t = static_cast<double>(idx) / SamplingRate;

            // 指数減衰
            double env = static_cast<double>(m.amplitude) * std::exp(-t / p.tau);

            // bandwidth による追加減衰
            if (m.bandwidth > 0.0f) {
                env *= std::exp(-m.bandwidth * t);
            }

            // harmonicMask による倍音削り
            env *= static_cast<double>(m.harmonicMask);

            if (env * std::abs(m.gain) < 1e-8) continue;

            // 位相 + ランダム揺らぎ
            double phaseJitter = 0.0;
            if (m.randomPhase > 0.0f) {
                phaseJitter = ((double)rand() / RAND_MAX * 2.0 - 1.0) * m.randomPhase;
            }
            double phase = p.phase0 + p.dphi * idx + phaseJitter;

            // サイン波
            double s = env * std::sin(phase);

            // ノイズ成分を混ぜる
            if (m.noiseMix > 0.0f) {
                double noise = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                s = (1.0 - m.noiseMix) * s + m.noiseMix * noise * env;
            }

            acc += s * m.gain;
        }

        // --- 非線形処理（ソフトクリップ） ---
        double clipAmount = (!prep.empty()) ? prep[0].mode->clipAmount : 0.0;
        if (clipAmount > 0.0) {
            acc = acc / (1.0 + clipAmount * std::abs(acc));
        }

        // --- フィルタリング ---
        double filtered = acc;
        if (!prep.empty()) {
            const ModalMode& m = *prep[0].mode;

            // ローパス (1次IIR近似)
            if (m.lowpassCutoff > 0.0f) {
                double rc = 1.0 / (2.0 * DirectX::XM_PI * m.lowpassCutoff);
                double dt = 1.0 / SamplingRate;
                double alpha = dt / (rc + dt);
                filtered = prevSample + alpha * (acc - prevSample);
            }

            // ハイパス (1次IIR近似)
            if (m.highpassCutoff > 0.0f) {
                double rc = 1.0 / (2.0 * DirectX::XM_PI * m.highpassCutoff);
                double dt = 1.0 / SamplingRate;
                double alpha = rc / (rc + dt);
                filtered = alpha * (prevSample + acc - prevSample);
            }
        }
        prevSample = filtered;

        // --- マスターゲイン適用、クリップ ---
        double v = filtered * masterGain * 32767.0;
        if (v > 32767.0) v = 32767.0;
        if (v < -32768.0) v = -32768.0;
        int16_t outSample = static_cast<int16_t>(std::lround(v));

        pushInt16LE(samples, outSample);
    }
    return samples;
}