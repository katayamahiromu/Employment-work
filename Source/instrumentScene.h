#pragma once
#include"Scene.h"
#include"system/ProceduralAudio.h"
#include<string>
#include<math.h>

class InstrumentScene : public Scene
{
public:
	InstrumentScene() {}
	~InstrumentScene() {}

	// 初期化
	void initialize()override;

	// 終了処理
	void finalize()override;

	// 更新処理
	void update(float elapsedTime)override;

	// 描画処理
	void render()override;

	void gui();
private:
	void playIndex(int index);
	void addInstrument(auto data,int addIndex);
private:
	std::unique_ptr<ProceduralAudio>instrument;
	std::unique_ptr<ProceduralAudio>Mu;
	const static int nodeMax = 8;

	float scale[nodeMax] = {
		261.63f, // C4
		293.66f, // D4
		329.63f, // E4
		349.23f, // F4
		392.00f, // G4
		440.00f, // A4
		493.88f, // B4
		523.25f  // C5
	};

	float phase[nodeMax] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

private:
	//ピッチ変更
	int basisOctave = 4;

	std::string keyArray[12] =
	{
		"C","Cis","D","Es","E","F",
		"Fis","G","As","A","B","H",
	};

	int key = 0;

	float freqMultiplier = 1.0f;

	const float octaveUp = 2.0f;
	const float octaveDawn = 0.5;
	const float SemitoneUp = powf(2.0f, 1.0f / 12.0f);
	const float SemitoneDown = powf(2.0f, -1.0f / 12.0f);
private:
	//音色変更
	enum class WaveType
	{
		Sine,
		Saw,
		Square,
		Triangle
	};

	enum class MixType
	{
		FM,
		RM,
		AM
	};

	std::vector<uint8_t>generateWave(WaveType type,float freq);
	std::vector<UINT8>mix(MixType type, waveData& data1,waveData& data2);

	WaveType currentWave = WaveType::Sine;
	WaveType modWave = WaveType::Sine;
	MixType mixType = MixType::FM;
};