#pragma once
#include<vector>
#include<DirectXMath.h>
#include <Windows.h>

class SoundGenerator
{
public:
    SoundGenerator() {};
    ~SoundGenerator(){}

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

    //ADSRエンベロープ
    float adsrEnvelope(float t, float duration,
        float attack, float decay,
        float sustainLevel, float release);
};

class SoundMixer
{
public:
    SoundMixer(int samplingRate = 44100);
    ~SoundMixer();

    // データ取得
    UINT8* getAudioData() { return data.data(); }

    // データサイズ取得
    UINT32 getAudioBytes() const { return static_cast<UINT32>(data.size()); }

    // WAVEフォーマット取得
    const WAVEFORMATEX& getWaveFormat() const { return wfx; }

    void addWave(const std::vector<uint8_t>& data, float frequency = 0.0f,float gain = 1.0f);
    void erase(int num);
    void clear() { waveArray.clear(); }
    int size() { return static_cast<int>(waveArray.size()); }
    void mix();

    void allFM(float modulationDepth);
    void applyFM(int carrierIndex, int modIndex, float modulationDepth,float gain = 1.0f);

    //選択された波形を再生できる形でdataを作る
    void selectData(int num);
private:
    struct waveData
    {
        std::vector<int16_t>samples;//PCM
        float gain = 1.0f; //ボリューム
        float frequency = 0.0f;//周波数
    };
private:
    WAVEFORMATEX wfx;
    std::vector<waveData>waveArray;
    std::vector<UINT8> data;
    int samplingRate = 44100;
};