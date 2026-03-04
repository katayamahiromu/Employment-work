#pragma once
#include"StereophonicParameter.h"

class BaseEmitter
{
public:
	BaseEmitter() {};
	virtual ~BaseEmitter() {}

	virtual void calcEmitterSetting(SoundEmitter& emitter) = 0;
	float calcFrequencyLPF(float filterPram);
	DSPResult&calcDSP(const SoundEmitter& emitter, const SoundListner& listener);
	void gui() {};
protected:
	//‘O‰ñ‚ÌˆÚ“®—Ê‚Ì·‚ğ‘¬“x‚Æ‚·‚é
	DirectX::XMFLOAT3 calcVelocity(const SoundEmitter& emitter);

	virtual float calcFilterParam(
		const SoundEmitter& emitter, 
		const SoundListner& listener,
		DSPResult& result) = 0;
protected:
	DirectX::XMFLOAT3 prevPosition = { 0.0f,0.0f,0.0f };
};