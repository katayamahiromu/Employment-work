#pragma once
#include<DirectXMath.h>
#include<functional>
#include<memory>
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
	waveData createData(const std::vector<uint8_t>& data, float frequency, float phase);

	void resize(int count) { waveArray.resize(count); }

	// データ取得
	UINT8* getAudioData() { return data.get()->data(); }

	// データサイズ取得
	UINT32 getAudioBytes() const { return static_cast<UINT32>(data.get()->size()); }

	std::shared_ptr<std::vector<UINT8>> allData() {return data; }

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
	void LowPass(int index,float cutoff,float depth);
	void BandPass(int index, float lowCut, float highCut);
public:
	waveData &getWData(int index) { return waveArray.at(index); }
private:
	WAVEFORMATEX wfx;
	std::shared_ptr<std::vector<UINT8>> data;
	std::vector<waveData>waveArray;
	int samplingRate;
};