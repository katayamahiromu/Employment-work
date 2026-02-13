#pragma once
#include<vector>
#include<DirectXMath.h>
#include <Windows.h>
#include"WaveData.h"
#include"ModalMode.h"
#include"math/DSPMath.h"

#include <array> 
#include <cstdint>
#include <random>

//入力に対しての波形を生成するクラス
class Oscillator
{
private:
    Oscillator() {
        //乱数生成
        std::seed_seq seq{ 1234567, 891011, 12131415, 16171819 };
        std::array<uint32_t, 8> seeds{};
        seq.generate(seeds.begin(), seeds.end());
        gustRng.state = _mm256_set_epi32(
            seeds[7], seeds[6], seeds[5], seeds[4],
            seeds[3], seeds[2], seeds[1], seeds[0]
        );
    };
    ~Oscillator() {};
public:
    static Oscillator* instance()
    {
        static Oscillator inst;
        return &inst;
    }

    //サイン波
    std::vector<uint8_t> sinWave(float frequency, float durationSeconds, int sampleRate = 44100);
    std::vector<uint8_t> sinWaveSIMD(float frequency, float durationSeconds, float& phase, int sampleRate = 44100);

    //ノコギリ波
    std::vector<uint8_t> sawtoothWave(float frequency, float durationSeconds, int sampleRate = 44100);
    std::vector<uint8_t> sawtoothWaveSIMD(float frequency, float durationSeconds, int sampleRate = 44100);

    //三角波
    std::vector<uint8_t> triangleWave(float frequency, float durationSeconds, int sampleRate = 44100);
    std::vector<uint8_t> triangleWaveSIMD(float frequency, float durationSeconds, int sampleRate = 44100);

    //矩形波
    std::vector<uint8_t> squareWave(float frequency, float durationSeconds, int sampleRate = 44100);
    std::vector<uint8_t> squareWaveSIMD(float frequency, float durationSeconds, int sampleRate = 44100);

    //ホワイトノイズ
    std::vector<uint8_t> whiteNoise(float durationSeconds, int sampleRate = 44100);
    std::vector<uint8_t> whiteNoiseSIMD(float durationSeconds, int sampleRate = 44100);

    //衝撃音
    std::vector<uint8_t> impactSound(float intensity, float durationSeconds, int sampleRate = 44100);

    //モードから衝撃音を作成
    std::vector<uint8_t> impactModes(const ModalMode* modes, size_t modeCount,float durationSeconds,float masterGain = 1.0);

    //乱流ノイズ
    std::vector<uint8_t>turbulenceNoise(
        float durationSeconds,
        int sampleRate,
        float windSpeed,      // 基本風速
        float gustAmount,     // ガスト（低周波揺らぎ）の強さ
        float brightness      // 高域の出やすさ（0.0〜1.0）
    );

    std::vector<uint8_t>turbulenceNoiseSIMD(
        float durationSeconds,
        int sampleRate,
        float windSpeed,      // 基本風速
        float gustAmount,     // ガスト（低周波揺らぎ）の強さ
        float brightness      // 高域の出やすさ（0.0〜1.0）
    );

    //ブラウンノイズ
    std::vector<uint8_t> brownNoise(float durationSeconds, int sampleRate, float gain);
    
    //ピンクノイズ
    std::vector<uint8_t> pinkNoise(float durationSeconds, int sampleRate, float gain);

    //バンドパスノイズ
    std::vector<uint8_t> bandpassNoise(
        float durationSeconds,
        int sampleRate,
        float centerFreq,
        float Q,
        float gain);

    //バブル粒子
    std::vector<uint8_t> bubbleNoise(
        float durationSeconds,
        int sampleRate,
        float density,      // 1秒あたりの泡の数
        float maxAmp        // 最大振幅
    );

    //レゾナンスノイズ
    std::vector<uint8_t> resonanceNoise(
        float durationSeconds,
        int sampleRate,
        const std::vector<float>& freqs, // 共鳴周波数のリスト
        float Q,
        float gain
    );

private:
    float pinkPrev = 0.0f;
    float lpPrev = 0.0f;
    float gustPrev = 0.0f;
    DSP::XorShift32x8 gustRng;
};