#pragma once
#include"WaveData.h"
#include"math/DSPMath.h"
#include<random>

class WaveShaper
{
private:
	WaveShaper(){
        rng.state = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    }
	~WaveShaper(){}
public:
    static WaveShaper* instance()
    {
        static WaveShaper inst;
        return &inst;
    }

    std::vector<UINT8> Decay(waveData& wave, float decay);
    std::vector<UINT8> LowPass(waveData& wave,float baseCutoff, float depth);
    std::vector<UINT8> BandPass(waveData& wave, float lowCut, float highCut);
    std::vector<UINT8> HighPass(waveData& wave, float cutoff, float resonance);
    //2次バターワース
    std::vector<UINT8> HighPass2nd(waveData& wave, float cutoff);
    std::vector<UINT8> WindHiss(waveData& wave, float St,     // ストローハル数
        float D,      // 物体サイズ（隙間）
        float U0,     // 基本風速
        float rQ,     // 共鳴の鋭さ
        float windRange); // 風速揺らぎ幅)

    std::vector<UINT8>WindHissSIMD(waveData& wave,
        float St,     // ストローハル数
        float D,      // 物体サイズ（隙間）
        float U0,     // 基本風速
        float rQ,     // 共鳴の鋭さ
        float windRange); // 風速揺らぎ幅

    std::vector<UINT8>AmplitudeJitter(waveData& wave,
        float jitterAmount); // ±何％揺らすか

    //データの先頭からms秒かけてフェードインする
    void fadeIn(std::vector<int16_t>& data, float ms);

    //データの後ろからmsかけてfadeOutする
    void fadeOut(std::vector<int16_t>& data, float ms);

    //クロスフェード
    void crossFade(std::vector<int16_t>& data, float ms);
private:
    //WindHissに必要なパラメータ
    float y1 = 0.0f, y2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f, b0 = 0.0f;
    float a1_t = 0.0f, a2_t = 0.0f, b0_t = 0.0f;
    int counter = 0;
    float wind = 0.0f;
    DSP::XorShift32x8 rng;
}; 