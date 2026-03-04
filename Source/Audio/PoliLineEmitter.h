#pragma once
#include"BaseEmitter.h"

class PoliLine:public BaseEmitter
{
public:
	PoliLine(std::vector<DirectX::XMFLOAT3> pointArray);
	~PoliLine() {};
	void calcEmitterSetting(SoundEmitter& emitter)override;
private:
	float calcFilterParam(const SoundEmitter& emitter, const SoundListner& listener, DSPResult& result)override;

	//点を繋いだ線分上に一番近いリスナーの位置を返す
	DirectX::XMFLOAT3 calcPos(DirectX::XMFLOAT3 listennerPos,DirectX::XMFLOAT3 emitterPos);

	const int SUBDIV = 20; // 分割数
	std::vector<DirectX::XMFLOAT3>pointArray;
};