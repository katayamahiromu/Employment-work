#pragma once
#include"Scene.h"
#include"Audio/AudioManager.h"
#include"Audio/SignalProcessor.h"
#include"Audio/ModalMode.h"
#include"../Utils/josn.h"

class AudioDemoScene : public Scene
{
public:
	AudioDemoScene();
	~AudioDemoScene();

	//‰Šú‰»ˆ—
	void initialize()override;

	//I—¹‰»
	void finalize()override;

	//XVˆ—
	void update(float elapsedTime)override;

	//•`‰æˆ—
	void render()override;
private:
	void gui();

	void ProceduralAudioGui();
	void importData();
	void inputModalGui();
private:
	void toJson(json& j, const ModalMode& m);
private:
	IXAudio2SourceVoice* source = nullptr;
	std::unique_ptr<SignalProcesser>signalProcess;

	float frequency = 440.0f;
	float durationSeconds = 0.5f;
	float gain = 1.0f;

	float modulationDepth = 1.0f;

	int carrierIndex = 0;
	int modIndex = 0;
	float decay = 1.0f;
	enum class WaveType :int
	{
		Sine = 0,
		Saw,
		Triangle,
		Square,
		Noise,
		Impact,
		Count
	};
	WaveType uiState;

	enum class modalTyepe :int
	{
		preset1,
		preset2,
		preset3,
		create,
	};
	modalTyepe modalState;

	std::vector<ModalMode>modals;

	char fileName[32];
};