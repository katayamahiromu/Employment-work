#pragma once
#include"WaveData.h"

class WaveShaper
{
private:
	WaveShaper(){}
	~WaveShaper(){}
public:
    static WaveShaper* instance()
    {
        static WaveShaper inst;
        return &inst;
    }

    std::vector<UINT8> Decay(waveData& wave, float decay);
};