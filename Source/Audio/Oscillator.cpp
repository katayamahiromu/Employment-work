#include"Oscillator.h"
#include<random>
#include <algorithm>
#include<math.h>
#include"SoundHealper.h"

std::vector<uint8_t> Oscillator::sinWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t>samples(numSamples * 2);

    double w = DirectX::XM_2PI * frequency / sampleRate;
    double phase = 0.0;

    for (int i = 0; i < numSamples; ++i)
    {
        int16_t s = static_cast<int16_t>(30000.0 * std::sin(phase));
        phase += w;
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::sinWaveSIMD(float frequency, float durationSeconds, float& phase,int sampleRate)
{
    const int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    // 位相は float ベースで管理
    const float w = DirectX::XM_2PI * frequency / static_cast<float>(sampleRate);

    uint8_t* dst = samples.data();

    // 8 サンプル分の位相増分
    const DSP::Sample8F w8 = _mm256_set1_ps(w);
    // ループ内で使う位相ベクトル
    DSP::Sample8F phase8_base = _mm256_set1_ps(phase);

    int i = 0;

    for (; i + 7 < numSamples; i += 8)
    {
        //波形生成
        DSP::Sample8F wave = DSP::SinWave(phase8_base,w8);

        //データ書き込み
        DSP::StorePCMData(dst,wave, 16);
        
        //位相更新
        phase8_base = DSP::UpdatePhase2PI(phase8_base,w8);
    }

    alignas(32) float phaseArray[8];
    _mm256_store_ps(phaseArray, phase8_base);
    phase = phaseArray[7]; //最後の位相を返す

  
    return samples;
}

std::vector<uint8_t> Oscillator::sawtoothWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    // 位相は 0.0 ～ 1.0 の範囲で回す
    double phase = 0.0;
    double phaseInc = frequency / sampleRate;   // 1サンプルあたりの位相増分

    for (int i = 0; i < numSamples; ++i)
    {
        // Saw波: -1.0 ～ +1.0
        double value = 2.0 * (phase - 0.5);

        // 16bit PCM にスケール
        int16_t s = static_cast<int16_t>(30000.0 * value);

        pushInt16LE(samples, s);

        // 位相更新
        phase += phaseInc;
        if (phase >= 1.0)
            phase -= 1.0;
    }

    return samples;
}

std::vector<uint8_t> Oscillator::sawtoothWaveSIMD(float frequency, float durationSeconds, int sampleRate)
{
    const int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    double phaseBase = 0.0;
    const double phaseInc = frequency / sampleRate;

    uint8_t* dst = samples.data();

    const  DSP::Sample4D inc4d = _mm256_set1_pd(phaseInc);
    DSP::Sample4D base4d = _mm256_set1_pd(phaseBase);

    int i = 0;
    for (; i + 7 < numSamples; i += 8)
    {
        //ノコギリ波生成
        DSP::Sample8F wave = DSP::SawWave(base4d, inc4d);

        //データ格納
        DSP::StorePCMData(dst, wave, 16);

        // 位相更新
        base4d = DSP::UpdatePhase4d(base4d, inc4d);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::triangleWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    // sinWave と同じ構造：w を使う
    double w = frequency / sampleRate;   // 位相増分（0..1）
    double phase = 0.0;                  // 0..1 の位相

    for (int i = 0; i < numSamples; ++i)
    {
        // 三角波生成（-1..1）
        // 0..1 の位相を -1..1 の三角波に変換
        double tri = 2.0 * std::fabs(2.0 * (phase - std::floor(phase + 0.5))) - 1.0;

        int16_t s = static_cast<int16_t>(30000.0 * tri);
        pushInt16LE(samples, s);

        // 位相更新
        phase += w;
        if (phase >= 1.0)
            phase -= 1.0;
    }

    return samples;
}

std::vector<uint8_t> Oscillator::triangleWaveSIMD(float frequency, float durationSeconds, int sampleRate)
{
    const int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    const float w = frequency / sampleRate;

    uint8_t* dst = samples.data();

    // phase8 = [0*w, 1*w, 2*w, ..., 7*w]
    DSP::Sample8F phase8 = _mm256_mul_ps(DSP::idx8, _mm256_set1_ps(w));
    const DSP::Sample8F inc8 = _mm256_set1_ps(w);

    int i = 0;
    for (; i + 7 < numSamples; i += 8)
    {
        // 三角波生成（-1..1 → PCM スケール）
        DSP::Sample8F wave = DSP::TriangleWave(phase8);
        DSP::StorePCMData(dst, wave, 16);
        phase8 = DSP::UpdatePhase(phase8, inc8);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::squareWave(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    float w = frequency / sampleRate;  // 位相増分（0..1）
    float phase = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // square 波：phase < 0.5 なら +1、そうでなければ -1
        float value = (phase < 0.5f) ? 1.0f : -1.0f;

        int16_t s = static_cast<int16_t>(value * 30000.0f);
        pushInt16LE(samples, s);

        // 位相更新
        phase += w;
        if (phase >= 1.0f)
            phase -= 1.0f;
    }

    return samples;
}

std::vector<uint8_t> Oscillator::squareWaveSIMD(float frequency, float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    const float w = frequency / sampleRate;

    __m256 phase = DSP::PhaseAmount(w);

    const __m256 inc8 = _mm256_set1_ps(8.0f * w);
    const __m256 negOne = _mm256_set1_ps(-1.0f);

    uint8_t* dst = samples.data();
    int i = 0;

    for (; i + 8 <= numSamples; i += 8)
    {
        //
        // square = (phase < 0.5) ? +1 : -1
        //
        __m256 mask = _mm256_cmp_ps(phase, DSP::half, _CMP_LT_OS);
        __m256 sq = _mm256_blendv_ps(negOne, DSP::one, mask);

        //
        // scale to int16 range
        //
        __m256 scaled = _mm256_mul_ps(sq, DSP::scale8);

        //
        // store 8 samples (16 bytes)
        //
        DSP::Avx2SClampedF32To16(dst, scaled);
        dst += 16;

        //
        // 位相更新
        //
        phase = _mm256_add_ps(phase, inc8);

        // wrap: phase -= floor(phase)
        __m256 wrap = _mm256_floor_ps(phase);
        phase = _mm256_sub_ps(phase, wrap);
    }

    //
    // scalar tail
    //
    float phaseScalar = i * w;
    for (; i < numSamples; ++i)
    {
        float value = (phaseScalar < 0.5f) ? 1.0f : -1.0f;
        int16_t s = static_cast<int16_t>(value * 30000.0f);
        pushInt16LE(samples, s);

        phaseScalar += w;
        if (phaseScalar >= 1.0f)
            phaseScalar -= 1.0f;
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

std::vector<uint8_t> Oscillator::whiteNoiseSIMD(float durationSeconds, int sampleRate)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    uint8_t* dst = samples.data();

    DSP::XorShift32x8 rng;
    rng.state = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);

    int i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        __m256i r = DSP::xs32_next(rng);

        // int32 → float
        __m256 rf = _mm256_cvtepi32_ps(r);

        // -1..1 に正規化
        rf = _mm256_mul_ps(rf, _mm256_set1_ps(1.0f / 2147483648.0f));

        // ±30000 にスケール
        __m256 scaled = _mm256_mul_ps(rf, DSP::scale8);

        DSP::StorePCMData(dst, scaled, 16);
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
        double value = envelope * std::sin(DirectX::XM_2PI * freq * t); // -1..1 (scaled by envelope)
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

std::vector<uint8_t> Oscillator::turbulenceNoise(
    float durationSeconds,
    int sampleRate,
    float windSpeed,      // 0.0〜10.0 くらい
    float gustAmount,     // 0.0〜1.0
    float brightness      // 0.0〜1.0
)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float pinkPrev = 0.0f;
    float lpPrev = 0.0f;
    float gustPrev = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // --- ホワイトノイズ ---
        float white = dist(rng);

        // --- ピンクノイズ ---
        float pink = white + 0.98f * pinkPrev;
        pinkPrev = pink;

        // --- ガスト（低周波揺らぎ） ---
        gustPrev = gustPrev * 0.995f + dist(rng) * 0.005f;
        float gust = 1.0f + gustPrev * gustAmount;

        // --- ローパス（風速で帯域が変わる） ---
        float cutoff = 200.0f + windSpeed * 80.0f + brightness * 3000.0f;
        float wc = DirectX::XM_2PI * cutoff / sampleRate;
        float a = wc / (wc + 1.0f);

        float filtered = a * pink + (1.0f - a) * lpPrev;
        lpPrev = filtered;

        //風速で音量を調整
        float gain = windSpeed * 0.03f;
        float out = filtered * gust * gain;

        //int16 に変換
        int16_t s = static_cast<int16_t>(std::clamp(out * 20000.0f, -32768.0f, 32767.0f));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::turbulenceNoiseSIMD(
    float durationSeconds,
    int sampleRate,
    float windSpeed,      // 0.0〜10.0 くらい
    float gustAmount,     // 0.0〜1.0
    float brightness      // 0.0〜1.0
)
{
    std::vector<uint8_t> white = whiteNoiseSIMD(durationSeconds);
    const int numSamples = static_cast<int>(white.size());

    std::vector<uint8_t> samples(numSamples * 2);
    uint8_t* dst = samples.data();


    // 定数
    const float cutoff = 200.0f + windSpeed * 80.0f + brightness * 3000.0f;
    const float wc = DirectX::XM_2PI * cutoff / sampleRate;
    const float a = wc / (wc + 1.0f);

    const float gain = windSpeed * 0.03f;

    const DSP::Sample8F kGain = DSP::loadFloat(gain);
    const DSP::Sample8F kScale = DSP::scale8; 
    const DSP::Sample8F kInv127 = DSP::loadFloat(1.0f / 127.5f);
    const DSP::Sample8F kBias = DSP::nOne;   // -1.0f

    // gust IIR 用係数
    const float gustA = 0.995f;
    const float gustB = 0.005f * gustAmount;

    int i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        DSP::Sample8F w_u8 = DSP::loadFloat8(&white[i]);

        DSP::Sample8F w = _mm256_add_ps(_mm256_mul_ps(w_u8, kInv127), kBias);

        // 8 サンプル分を一旦スカラー配列に落とす
        alignas(32) float wArr[8];
        _mm256_store_ps(wArr, w);

        float pinkArr[8];
        float gustArr[8];

        // pink[n] = white[n] + 0.98 * pink[n-1]
        for (int k = 0; k < 8; ++k)
        {
            float pink = wArr[k] + 0.98f * pinkPrev;
            pinkPrev = pink;
            pinkArr[k] = pink;
        }

        // Gust ノイズ（低周波揺らぎ
        // gustPrev = gustPrev * 0.995 + rand(-1..1) * 0.005 * gustAmount;
        // gust = 1.0 + gustPrev;
        {
            // SIMD で 8 個乱数を生成（0〜1 の float にしてから -1〜1 へ）
            __m256i r32 = DSP::xs32_next(gustRng);

            alignas(32) uint32_t rArr[8];
            _mm256_store_si256((__m256i*)rArr, r32);

            for (int k = 0; k < 8; ++k)
            {
                // 0〜1 の float に正規化（2^32 分の 1）
                float rf = (float)(rArr[k] * (1.0 / 4294967296.0)); // [0,1)
                float rf2 = rf * 2.0f - 1.0f;                       // [-1,1)

                gustPrev = gustPrev * gustA + rf2 * gustB;
                gustArr[k] = 1.0f + gustPrev;
            }
        }

        DSP::Sample8F pink = _mm256_load_ps(pinkArr);
        DSP::Sample8F gust = _mm256_load_ps(gustArr);
        DSP::Sample8F filtered = DSP::Lowpass(pink, a, lpPrev);

        // Gain & gust 適用
        DSP::Sample8F out = _mm256_mul_ps(_mm256_mul_ps(filtered, gust), kGain);

        //スケーリング & クリップ
        DSP::Sample8F scaled = _mm256_mul_ps(out, kScale);
        DSP::Sample8F clipped = DSP::clampPCM(scaled);

        DSP::StorePCMData(dst, clipped, 16);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::brownNoise(float durationSeconds, int sampleRate, float gain)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float brown = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        brown += dist(gen) * 0.02f;     // 積分
        brown = std::clamp(brown, -1.0f, 1.0f);

        int16_t s = static_cast<int16_t>(brown * gain);
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::pinkNoise(float durationSeconds, int sampleRate, float gain)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    const int numRows = 16;
    int rows[numRows] = { 0 };
    int counter = 0;

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> dist(-30000, 30000);

    for (int i = 0; i < numSamples; ++i)
    {
        int lastCounter = counter;
        counter++;

        int sum = 0;

        for (int r = 0; r < numRows; ++r)
        {
            if ((counter & (1 << r)) != (lastCounter & (1 << r)))
            {
                rows[r] = dist(gen);
            }
            sum += rows[r];
        }

        int16_t s = static_cast<int16_t>((sum / numRows) * gain / 30000.0f);
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::bandpassNoise(
    float durationSeconds,
    int sampleRate,
    float centerFreq,
    float Q,
    float gain)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    // ホワイトノイズ生成器
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // BPF 設計
    float w0 = 2.0f * 3.14159265f * centerFreq / sampleRate;
    float alpha = sinf(w0) / (2.0f * Q);

    float b0 = alpha;
    float b1 = 0.0f;
    float b2 = -alpha;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosf(w0);
    float a2 = 1.0f - alpha;

    // 正規化
    b0 /= a0; b1 /= a0; b2 /= a0;
    a1 /= a0; a2 /= a0;

    float x1 = 0, x2 = 0;
    float y1 = 0, y2 = 0;

    for (int i = 0; i < numSamples; ++i)
    {
        float x0 = dist(gen);

        float y0 = b0 * x0 + b1 * x1 + b2 * x2
            - a1 * y1 - a2 * y2;

        x2 = x1; x1 = x0;
        y2 = y1; y1 = y0;

        int16_t s = static_cast<int16_t>(y0 * gain);
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::bubbleNoise(
    float durationSeconds,
    int sampleRate,
    float density,      // 1秒あたりの泡の数
    float maxAmp        // 最大振幅
)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);

    // まず全サンプルを 0 で埋める
    std::vector<int16_t> temp(numSamples, 0);

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> ampDist(0.1f, maxAmp);
    std::uniform_real_distribution<float> timeDist(0.0f, durationSeconds);
    std::uniform_real_distribution<float> durDist(0.001f, 0.005f); // 1〜5ms

    int numBubbles = static_cast<int>(durationSeconds * density);

    for (int b = 0; b < numBubbles; ++b)
    {
        float startTime = timeDist(gen);
        float bubbleDur = durDist(gen);
        float amp = ampDist(gen);

        int startSample = static_cast<int>(startTime * sampleRate);
        int bubbleSamples = static_cast<int>(bubbleDur * sampleRate);

        for (int i = 0; i < bubbleSamples; ++i)
        {
            int idx = startSample + i;
            if (idx >= numSamples) break;

            float env = 1.0f - (float)i / bubbleSamples; // 短い減衰
            float s = amp * env;

            int16_t v = static_cast<int16_t>(s * 30000);

            // temp にミックス（加算）
            temp[idx] = std::clamp<int>(temp[idx] + v, -32768, 32767);
        }
    }

    // 最後に pushInt16LE でバイト列に変換
    std::vector<uint8_t> samples;
    samples.reserve(numSamples * 2);

    for (int i = 0; i < numSamples; ++i)
    {
        pushInt16LE(samples, temp[i]);
    }

    return samples;
}

std::vector<uint8_t> Oscillator::resonanceNoise(
    float durationSeconds,
    int sampleRate,
    const std::vector<float>& freqs, // 共鳴周波数のリスト
    float Q,
    float gain
)
{
    int numSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<uint8_t> samples(numSamples * 2);

    // ホワイトノイズ生成器
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // 各フィルタの状態
    struct BPFState { float x1 = 0, x2 = 0, y1 = 0, y2 = 0; };
    std::vector<BPFState> states(freqs.size());

    // フィルタ係数を事前計算
    struct BPF { float b0, b1, b2, a1, a2; };
    std::vector<BPF> filters;

    for (float f : freqs)
    {
        float w0 = 2.0f * 3.14159265f * f / sampleRate;
        float alpha = sinf(w0) / (2.0f * Q);

        float b0 = alpha;
        float b1 = 0.0f;
        float b2 = -alpha;
        float a0 = 1.0f + alpha;
        float a1 = -2.0f * cosf(w0);
        float a2 = 1.0f - alpha;

        filters.push_back({
            b0 / a0, b1 / a0, b2 / a0,
            a1 / a0, a2 / a0
            });
    }

    // LFO（ゆっくり揺らす）
    float lfoPhase = 0.0f;
    float lfoSpeed = 0.3f; // Hz

    for (int i = 0; i < numSamples; ++i)
    {
        float x = dist(gen);

        float out = 0.0f;

        // LFO
        float lfo = 0.5f + 0.5f * sinf(lfoPhase);
        lfoPhase += (2.0f * 3.14159265f * lfoSpeed) / sampleRate;

        // 各共鳴フィルタを通す
        for (size_t k = 0; k < filters.size(); ++k)
        {
            auto& f = filters[k];
            auto& st = states[k];

            float y = f.b0 * x + f.b1 * st.x1 + f.b2 * st.x2
                - f.a1 * st.y1 - f.a2 * st.y2;

            st.x2 = st.x1; st.x1 = x;
            st.y2 = st.y1; st.y1 = y;

            out += y * lfo; // LFOで揺らす
        }

        int16_t s = static_cast<int16_t>(out * gain);
        pushInt16LE(samples, s);
    }

    return samples;
}

