#pragma once
#include"Component.h"
#include"Audio/Audio3DSystem.h"

class Audio3DEmitter : public Component
{
public:
	Audio3DEmitter(float minDis,float maxDis);
	~Audio3DEmitter();


	const char* getName() const override { return "Audio3DEmitter"; }

	void prepare()override;
	void update(float elapsedTime)override;
	void OnGUI();

	void DSPSetting(int slot);
	void loadFile(const char*filename);
	void setVelocity(DirectX::XMFLOAT3 v) { emitter.velocity = v; }
	void play(int slot);
private:
	SoundEmitter emitter;
	std::unique_ptr<Audio3D>source;
};