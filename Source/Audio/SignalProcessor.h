#pragma once
#include<DirectXMath.h>
#include<functional>
#include"WaveData.h"

enum class WaveTypeInfo
{
	Sin,
	Saw,
	Triangle,
	Squere,
	Noise,
};

class SignalProcesser
{
public:
	SignalProcesser();
	~SignalProcesser();

	//波形の追加
	void addWave(const std::vector<uint8_t>& data, float frequency = 0.0f, float gain = 1.0f);
	void addWave(const std::vector<uint8_t>& data, float frequency, float gain,int index);

	waveData createData(const std::vector<uint8_t>& data);
	void resize(int count) { waveArray.resize(count); }
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

	//作成する波形情報のセット
	void setWaveTypeData(WaveTypeInfo type, float frequency, float durationSeconds) { WaveInfoData = { type,frequency ,durationSeconds }; }

	//波形のレンダリング
	void render(float durationSeconds);
public:
	//合成
	void Mix();
	void applyFM(int carriarIndex, int modIndex, float modulationDepth, float gain = 1.0f);
	void applyAM(int carriarIndex, int modIndex, float modulationDepth, float gain = 1.0f);
	void applyRM(int carriarIndex, int modIndex,float modulationDepth, float gain = 1.0f);

	//減算
	void decayWave(int index, float decay);
	void LowPass(int index,float cutoff,float depth);
	void BandPass(int index, float lowCut, float highCut);

private:
	struct WaveTypeData
	{
		WaveTypeInfo type;
		float frequency;
		float durationSeconds;
	};
public:
	waveData &getWData(int index) { return waveArray.at(index); }
private:
	WaveTypeData WaveInfoData;
	WAVEFORMATEX wfx;
	std::vector<waveData>waveArray;
	std::vector<UINT8> data;
	int samplingRate;
};