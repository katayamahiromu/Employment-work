#pragma once
#include"Component.h"
#include"Audio/Audio3DSystem.h"
#include"Audio/StereophonicParameter.h"
#include"Audio/AudioManager.h"

class Audio3DEmitter : public Component
{
public:
	Audio3DEmitter(float minDis,float maxDis);
	~Audio3DEmitter();
	void setListenerType(int type) { listener = AudioManager::instance()->findAudio(type); }
	const char* getName() const override { return "Audio3DEmitter"; }

	void prepare()override;
	void update(float elapsedTime)override;
	void OnGUI();

	void loadFile(const char* filename);
	void play(int slot);
private:
	SoundEmitter emitter;
	SoundListner* listener = nullptr;
	std::unique_ptr<Audio3D>source;
};