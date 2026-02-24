#pragma once
#include"WaveData.h"

//波形の合成を行うクラス
class SignalMixer
{
private:
    SignalMixer() {};
    ~SignalMixer() {};
public:
    static SignalMixer* instance()
    {
        static SignalMixer inst;
        return &inst;
    }

    std::vector<UINT8> mix(std::vector<waveData>&waveArray);

    //FM合成
    std::vector<UINT8> frequencyModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);
    std::vector<UINT8> frequencyModulation(waveData& carrierData, float modulationDepth, float gain = 1.0f);

    //AM合成
    std::vector<UINT8> amplitudeModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);
    std::vector<UINT8> amplitudeModulation(waveData& carrierData,float modulationDepth, float lfoFreq, float gain = 1.0f);

    //RM合成
    std::vector<UINT8> ringModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);

    //LFO
    std::vector<UINT8> vibratoLFO(waveData& carrierData, float semitones, float lfoFreq, float gain);

    std::vector<UINT8> granularize(
        waveData& src,
        float grainMs,        // グレイン長（ms）
        float densityHz,      // 1秒あたりのグレイン数（密度）
        float pitchRandSemis, // ピッチランダム（±半音）
        float posJitterMs,    // 出力配置ジッター（ms）
        float gain);           // 出力ゲイン

};