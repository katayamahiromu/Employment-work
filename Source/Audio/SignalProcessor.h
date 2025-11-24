#pragma once
#include<DirectXMath.h>
#include"WaveData.h"

class SignalProcesser
{
public:
	SignalProcesser();
	~SignalProcesser();

	//波形の追加
	void addWave(const std::vector<uint8_t>& data, float frequency = 0.0f, float gain = 1.0f);

	// データ取得
	UINT8* getAudioData() { return data.data(); }

	// データサイズ取得
	UINT32 getAudioBytes() const { return static_cast<UINT32>(data.size()); }

	// WAVEフォーマット取得
	const WAVEFORMATEX& getWaveFormat() const { return wfx; }

	//削除	基本デバック機能
	void erase(int num){ waveArray.erase(waveArray.begin() + num); }

	//配列内に存在する波形を再生できる形にするデバック機能
	void trySingleWave(int num);

	void clear() { waveArray.clear(); }
	int size() { return static_cast<int>(waveArray.size()); }
public:
	//合成
	void Mix();
	void applyFM(int carriarIndex, int modIndex, float modulationDepth, float gain = 1.0f);
	void applyAM(int carriarIndex, int modIndex, float modulationDepth, float gain = 1.0f);
	void applyRM(int carriarIndex, int modIndex,float modulationDepth, float gain = 1.0f);

	//減算
	void decayWave(int index, float decay);
private:
	WAVEFORMATEX wfx;
	std::vector<waveData>waveArray;
	std::vector<UINT8> data;
	int samplingRate;
};