#pragma once
#include"BaseEmitter.h"

class PointEmitter : public BaseEmitter
{
public:
	PointEmitter(DirectX::XMFLOAT3* emitterPosition);
	~PointEmitter() {}
	void calcEmitterSetting(SoundEmitter& emitter)override;
private:
	float calcFilterParam(const SoundEmitter& emitter, const SoundListner& listener, DSPResult& result)override;
	DirectX::XMFLOAT3* position;
};