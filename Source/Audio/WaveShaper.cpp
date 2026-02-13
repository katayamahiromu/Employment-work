#include"WaveShaper.h"
#include"SoundHealper.h"
#include<algorithm>
#include<DirectXMath.h>
#include<random>
#include"math/DSPMath.h"

std::vector<UINT8> WaveShaper::Decay(waveData& wave, float decay)
{
	std::vector<UINT8> samples;

	if (wave.samples.empty())return {};

	size_t numSamples = wave.samples.size();
	samples.reserve(numSamples * 2);

	for (size_t i = 0;i < numSamples;++i)
	{
		float t = static_cast<float>(i) / SamplingRate;

		// 指数減衰エンベロープ
		float envelope = exp(-decay * t);

		// 減衰処理
		float v = static_cast<float>(wave.samples[i]) * envelope;

		// 16bit に収める
		int16_t s = static_cast<int16_t>(std::clamp(v, -32768.0f, 32767.0f));
		pushInt16LE(samples, s);
	}

	return samples;
}

std::vector<UINT8> WaveShaper::LowPass(waveData& wave, float baseCutoff, float depth)
{
    std::vector<UINT8> samples;

    if (wave.samples.empty()) return {};

    size_t numSamples = wave.samples.size();
    samples.reserve(numSamples * 2);

    // Smooth Random LFO
    float lfoValue = 0.0f;
    float smooth = 0.995f; // 0.99〜0.999 が自然

    std::mt19937 rng{ 12345 };
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float z = 0.0f; // フィルタ内部状態

    for (size_t i = 0; i < numSamples; ++i)
    {
        // --- LFO 更新 ---
        float r = dist(rng);
        lfoValue = smooth * lfoValue + (1.0f - smooth) * r; // -1〜1 の滑らかなランダム

        // --- カットオフ計算 ---
        float cutoff = baseCutoff + depth * lfoValue;
        cutoff = std::clamp(cutoff, 200.0f, 8000.0f);

        // --- LPF 係数 ---
        float a = std::exp(-DirectX::XM_2PI * cutoff / SamplingRate);
        float b = 1.0f - a;

        // --- 入力サンプル ---
        float x = static_cast<float>(wave.samples[i]);

        // --- 1-pole LPF ---
        z = b * x + a * z;

        // --- 16bit PCM に変換 ---
        int16_t s = static_cast<int16_t>(std::clamp(z, -32768.0f, 32767.0f));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<UINT8> WaveShaper::BandPass(waveData& wave, float lowCut, float highCut)
{
    std::vector<UINT8> samples;

    if (wave.samples.empty()) return {};

    size_t numSamples = wave.samples.size();
    samples.reserve(numSamples * 2);

    // --- HPF 係数 ---
    float aHP = std::exp(-DirectX::XM_2PI * lowCut / SamplingRate);
    float bHP = 1.0f - aHP;
    float zHP = 0.0f; // HPF 内部状態

    // --- LPF 係数 ---
    float aLP = std::exp(-DirectX::XM_2PI * highCut / SamplingRate);
    float bLP = 1.0f - aLP;
    float zLP = 0.0f; // LPF 内部状態

    for (size_t i = 0; i < numSamples; ++i)
    {
        float x = static_cast<float>(wave.samples[i]);

        // --- 1-pole HPF ---
        zHP = bHP * x + aHP * zHP;
        float y = x - zHP; // HPF 出力

        // --- 1-pole LPF ---
        zLP = bLP * y + aLP * zLP;
        float out = zLP;

        // --- 16bit PCM に変換 ---
        int16_t s = static_cast<int16_t>(std::clamp(out, -32768.0f, 32767.0f));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<UINT8> WaveShaper::WindHiss(waveData& wave,
    float St,     // ストローハル数
    float D,      // 物体サイズ（隙間）
    float U0,     // 基本風速
    float rQ,     // 共鳴の鋭さ
    float windRange // 風速揺らぎ幅
)
{
    std::vector<UINT8> samples;
    if (wave.samples.empty()) return {};

    size_t numSamples = wave.samples.size();
    samples.reserve(numSamples * 2);

    // --- 風速揺らぎ ---
    //float wind = U0;
    float windSmooth = 0.9995f;

    std::mt19937 rng{ 12345 };
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // --- 共鳴フィルタ状態 ---
    //float y1 = 0.0f, y2 = 0.0f;

    // --- フィルタ係数（スムージング用） ---
   /* float a1 = 0.0f, a2 = 0.0f, b0 = 0.0f;
    float a1_t = 0.0f, a2_t = 0.0f, b0_t = 0.0f;*/

    int updateInterval = SamplingRate / 1000;
    //int counter = 0;

    for (size_t i = 0; i < numSamples; ++i)
    {
        float x = static_cast<float>(wave.samples[i]) * 0.7f;

        // --- 係数更新 ---
        if (counter++ >= updateInterval)
        {
            counter = 0;

            float r = dist(rng);
            wind = windSmooth * wind + (1.0f - windSmooth) * (U0 + r * windRange);

            float f = St * (wind / D);
            float w0 = DirectX::XM_2PI * f / SamplingRate;

            b0_t = (1.0f - rQ * rQ);
            a1_t = -2.0f * rQ * std::cos(w0);
            a2_t = rQ * rQ;
        }

        // 係数スムージング ---
        float coefSmooth = 0.001f;
        a1 = a1 * (1.0f - coefSmooth) + a1_t * coefSmooth;
        a2 = a2 * (1.0f - coefSmooth) + a2_t * coefSmooth;
        b0 = b0 * (1.0f - coefSmooth) + b0_t * coefSmooth;

        // --- 2次共鳴フィルタ ---
        float y = b0 * x - a1 * y1 - a2 * y2;

        y2 = y1;
        y1 = y;

        int16_t s = static_cast<int16_t>(std::clamp(y, -32768.0f, 32767.0f));
        pushInt16LE(samples, s);
    }

    return samples;
}

std::vector<UINT8> WaveShaper::WindHissSIMD(
    waveData& wave,
    float St,     // ストローハル数
    float D,      // 物体サイズ（隙間）
    float U0,     // 基本風速
    float rQ,     // 共鳴の鋭さ
    float windRange // 風速揺らぎ幅
)
{
    if (wave.samples.empty()) return {};

    const size_t numSamples = wave.samples.size();
    std::vector<UINT8> samples(numSamples * 2);
    uint8_t* dst = samples.data();

    // --- 風速揺らぎ ---
    float wind = U0;
    const float windSmooth = 0.9995f;

    // 乱数
    /*DSP::XorShift32x8 rng;
    rng.state = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);*/

    // --- 2次共鳴フィルタ状態（1 本鎖） ---
    //float y1 = 0.0f, y2 = 0.0f;

    // フィルタ係数 ---
   /* float a1 = 0.0f, a2 = 0.0f, b0 = 0.0f;
    float a1_t = 0.0f, a2_t = 0.0f, b0_t = 0.0f;*/

    const int updateInterval = SamplingRate / 1000;
    //int counter = 0;

    const float coefSmooth = 0.001f;

    size_t i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        // --- 乱数 8 個生成（0〜1 → -1〜1） ---
        __m256i r32 = DSP::xs32_next(rng);
        alignas(32) uint32_t rArr[8];
        _mm256_store_si256((__m256i*)rArr, r32);

        float rFloat[8];
        for (int k = 0; k < 8; ++k)
        {
            // 0..1 → -1..1
            float rf = (float)(rArr[k] * (1.0 / 4294967296.0)); // [0,1)
            rFloat[k] = rf * 2.0f - 1.0f;
        }

        // --- 入力ロード（int16 → float × 8, 0.7 スケール） ---
        __m128i s16 = _mm_loadu_si128((const __m128i*) & wave.samples[i]);
        __m256i s32 = _mm256_cvtepi16_epi32(s16);
        __m256 xf = _mm256_mul_ps(_mm256_cvtepi32_ps(s32), _mm256_set1_ps(0.7f));

        alignas(32) float x8[8];
        _mm256_store_ps(x8, xf);

        float y8[8];

        // --- 8 サンプルを「スカラー版と同じロジック」で逐次処理 ---
        for (int k = 0; k < 8; ++k)
        {
            // 係数更新（1kHz ごと）
            if (counter++ >= updateInterval)
            {
                counter = 0;

                float r = rFloat[k]; // このサンプルでの乱数

                wind = windSmooth * wind + (1.0f - windSmooth) * (U0 + r * windRange);

                float f = St * (wind / D);
                float w0 = DirectX::XM_2PI * f / SamplingRate;

                b0_t = (1.0f - rQ * rQ);
                a1_t = -2.0f * rQ * std::cos(w0);
                a2_t = rQ * rQ;
            }

            // 係数スムージング（毎サンプル）
            a1 = a1 * (1.0f - coefSmooth) + a1_t * coefSmooth;
            a2 = a2 * (1.0f - coefSmooth) + a2_t * coefSmooth;
            b0 = b0 * (1.0f - coefSmooth) + b0_t * coefSmooth;

            // 2次共鳴フィルタ
            float x = x8[k];
            float y = b0 * x - a1 * y1 - a2 * y2;

            y2 = y1;
            y1 = y;

            y8[k] = y;
        }

        // --- float[8] → int16[8] SIMD 変換 ---
        __m256 yf = _mm256_load_ps(y8);

        __m256 minv = _mm256_set1_ps(-32768.0f);
        __m256 maxv = _mm256_set1_ps(32767.0f);
        yf = _mm256_min_ps(maxv, _mm256_max_ps(minv, yf));

        __m256i yi32 = _mm256_cvtps_epi32(yf);

        __m128i lo = _mm256_castsi256_si128(yi32);
        __m128i hi = _mm256_extracti128_si256(yi32, 1);
        __m128i packed = _mm_packs_epi32(lo, hi);

        _mm_storeu_si128((__m128i*)dst, packed);
        dst += 16;
    }

    return samples;
}

std::vector<UINT8> WaveShaper::AmplitudeJitter(waveData& wave,
    float jitterAmount)
{
    std::vector<UINT8> samples;

    if (wave.samples.empty()) return {};

    size_t numSamples = wave.samples.size();
    samples.reserve(numSamples * 2);

    std::mt19937 rng{ 98765 };
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (size_t i = 0; i < numSamples; ++i)
    {
        // int16_t の PCM をそのまま取得
        float x = static_cast<float>(wave.samples[i]);

        // ジッター係数
        float jitter = 1.0f + dist(rng) * jitterAmount;

        // 振幅ジッター適用
        x *= jitter;

        // 16bit に収める
        int16_t v = static_cast<int16_t>(std::clamp(x, -32768.0f, 32767.0f));

        // 出力へ書き込み（16bit LE）
        pushInt16LE(samples, v);
    }

    return samples;
}