#pragma once
#include <Windows.h>
#include<vector>

#define SamplingRate 44100

struct waveData
{
	std::vector<int16_t>samples;//PCM
	float gain = 1.0f; //ƒ{ƒŠƒ…[ƒ€
	float frequency = 0.0f;//ü”g”
};