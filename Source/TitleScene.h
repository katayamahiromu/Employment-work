#pragma once
#include"Button.h"
#include"Scene.h"
#include"Component/object.h"
#include"Audio/AudioManager.h"
#include"Graphics/Skymap.h"
#include"Component/Animation.h"
#include"Component/PlayerController.h"
#include<string>
#include<memory>

#include"Graphics/Specturm.h"
#include"system/PostprocessingRenderer.h"

#include"Audio/SignalProcessor.h"

//ƒ^ƒCƒgƒ‹ƒV[ƒ“
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//‰Šú‰»
	void initialize() override;

	//I—¹‰»
	void finalize()override;

	//XVˆ—
	void update(float elapsedTime)override;

	//•`‰æˆ—
	void render()override;

	void Gui();
private:
	void updateSelector(float elapsedTime);
	enum Select
	{
		Start,
		Exit,
	};
	int select = 0;
	float paddingTime = 0;

	//“ü—Í’â~ŠÔ
	const float freezeTime = 0.3f;
private:
	//
	std::unique_ptr<Camera>fixCamera;
	//3DŠÖŒW
	std::unique_ptr<ObjectManager>objManager;
	std::shared_ptr<Object>model;
	std::shared_ptr<Animation>animation;
	
	//“®‚«
	std::shared_ptr<PlayerController>controller;

	//2DŠÖŒW
	std::unique_ptr<Sprite>titleImage;
	std::unique_ptr<Button>rogo;

	std::vector<Button*>choices;

	std::unique_ptr<Audio>sample;
	std::unique_ptr<Audio>sampleRevers;
	std::unique_ptr<Spectrum>spectrum;

	std::unique_ptr<PostprocessingRenderer>PostEffects;

	float rate = 1.0f;

	//ŒŸ¸
	float wetLevel = 0.7f;
	float roomSize = 1.0f;
	float decayTime = 20.0f;

	float wetDryMix = 1.0; // 0.0f ` 100.0f
	float feedback = 0.0f; // -99.0f ` 99.0f
	float delay = 500.0f; // 1ms ` 2000ms

	IXAudio2SourceVoice* source = nullptr;
	std::unique_ptr<SignalProcesser>signalProcess;

	float frequency = 440;
	float durationSeconds = 0.5;
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
};