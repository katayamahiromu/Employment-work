#include"PointEmitter.h"
#include"math/Mathf.h"
#include<algorithm>

PointEmitter::PointEmitter(DirectX::XMFLOAT3* emitterPosition):position(emitterPosition)
{
}

void PointEmitter::calcEmitterSetting(SoundEmitter& emitter)
{
	//位置更新
	emitter.position = *position;

	//速度更新
	emitter.velocity = calcVelocity(emitter);

	//今回位置の保存
	prevPosition = emitter.position;
}

float PointEmitter::calcFilterParam(const SoundEmitter& emitter, const SoundListner& listener, DSPResult& result)
{
	//正面にあるか
	float front = std::abs(result.radian);

	if (front > listener.innerRadius)
	{
		//角度に応じてスケール
		float t = std::min(1.0f,(front - listener.innerRadius) / (listener.outerRadius - listener.innerRadius));
		
		//スケールに応じてフィルターパラメーターを調整
		return result.scale* t;
	}
	else
	{
		//正面ではフィルター無し
		return 0.0f;
	}
}