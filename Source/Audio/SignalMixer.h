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
    std::vector<UINT8> frequencyModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);
    std::vector<UINT8> amplitudeModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);
    std::vector<UINT8> ringModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain = 1.0f);
};