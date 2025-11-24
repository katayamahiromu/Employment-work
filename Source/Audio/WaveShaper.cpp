#include"WaveShaper.h"
#include"SoundHealper.h"
#include<algorithm>

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