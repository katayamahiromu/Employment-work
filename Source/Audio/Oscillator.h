#pragma once
#include<vector>
#include<DirectXMath.h>
#include <Windows.h>
#include"WaveData.h"
#include"ModalMode.h"

//入力に対しての波形を生成するクラス
class Oscillator
{
private:
    Oscillator() {};
    ~Oscillator() {};
public:
    static Oscillator* instance()
    {
        static Oscillator inst;
        return &inst;
    }

    //サイン波
    std::vector<uint8_t> sinWave(float frequency, float durationSeconds, int sampleRate = 44100);

    //ノコギリ波
    std::vector<uint8_t> sawtoothWave(float frequency, float durationSeconds, int sampleRate = 44100);

    //三角波
    std::vector<uint8_t> triangleWave(float frequency, float durationSeconds, int sampleRate = 44100);

    //矩形波
    std::vector<uint8_t> squareWave(float frequency, float durationSeconds, int sampleRate = 44100);

    //ホワイトノイズ
    std::vector<uint8_t> whiteNoise(float durationSeconds, int sampleRate = 44100);

    //衝撃音
    std::vector<uint8_t> impactSound(float intensity, float durationSeconds, int sampleRate = 44100);

    //モードから衝撃音を作成
    std::vector<uint8_t> impactModes(const ModalMode* modes, size_t modeCount,float durationSeconds,float masterGain = 1.0);
};