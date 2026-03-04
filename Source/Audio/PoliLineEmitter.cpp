#include"PoliLineEmitter.h"
#include"AudioManager.h"
#include"../math/Mathf.h"

PoliLine::PoliLine(std::vector<DirectX::XMFLOAT3> pointArray):pointArray(pointArray)
{
}

void PoliLine::calcEmitterSetting(SoundEmitter& emitter)
{
	//位置更新
	SoundListner *listner = AudioManager::instance()->findAudio(static_cast<int>(Lisner::PLAYER));
	emitter.position = calcPos(listner->position,emitter.position);

	//速度更新
	emitter.velocity = calcVelocity(emitter);

	//今回位置の保存
	prevPosition = emitter.position;
}

float PoliLine::calcFilterParam(const SoundEmitter& emitter, const SoundListner& listener, DSPResult& result)
{
	float front = std::abs(result.radian);

	if (front <= listener.innerRadius)
		return 0.0f;

	// 角度スケール（0〜1）
	float t = min(1.0f,(front - listener.innerRadius) /(listener.outerRadius - listener.innerRadius));

	// 線音源は指向性が弱いローパス量
	const float lineSourceFactor = 0.4f;

	// 距離スケール
	return result.scale * t * lineSourceFactor;
}

DirectX::XMFLOAT3 PoliLine::calcPos(DirectX::XMFLOAT3 listennerPos, DirectX::XMFLOAT3 emitterPos)
{

	float minDistSq = FLT_MAX;
	DirectX::XMFLOAT3 bestPoint = emitterPos;

	for (size_t i = 1; i < pointArray.size() - 2; ++i)
	{
		for (int s = 0; s < SUBDIV; ++s)
		{
			float t0 = (float)s / SUBDIV;
			float t1 = (float)(s + 1) / SUBDIV;

			DirectX::XMFLOAT3 p0 = Mathf::CatmullRom(pointArray[i - 1], pointArray[i], pointArray[i + 1], pointArray[i + 2], t0);
			DirectX::XMFLOAT3 p1 = Mathf::CatmullRom(pointArray[i - 1], pointArray[i], pointArray[i + 1], pointArray[i + 2], t1);

			DirectX::XMFLOAT3 cp = Mathf::ClosestPointOnSegment(p0, p1, listennerPos);

			float distSq = Mathf::calcDistanceSqXYZ(cp, listennerPos);

			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				bestPoint = cp;
			}
		}
	}

	// 最近接点を保存
	return bestPoint;
}