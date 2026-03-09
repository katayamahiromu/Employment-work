#pragma once
#include<vector>
#include"BaseEmitter.h"
#include"Audio.h"
#include"SubMixVoice.h"

class Audio3D : public Audio
{
public:
	Audio3D(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, std::shared_ptr<BaseEmitter>emitterType,SoundEmitter* emitter);
	~Audio3D();

	void update3D(SoundListner& listener)override;

	void gui();

	DirectX::XMFLOAT3 getEmitterPos() { return emitter->position; }
private:
	void calcPan(DSPResult&result);
	void filter(XAUDIO2_FILTER_TYPE type, float filterParam, FLOAT32 overq = 1.0f);
	void updateReverb(DSPResult& result);
	void updateEcho(DSPResult& result);
	void updateHrtf(DSPResult& result);
private:
	SoundEmitter*emitter;
	SoundDSPSetting dspSetting;
	std::shared_ptr<BaseEmitter>emitterType;

	std::shared_ptr<SubMixVoice>effect;
	std::vector<std::shared_ptr<SubMixVoice>>hrtfArray;

	float angle = 0.0f;
	float panPower = 1.0f;
};