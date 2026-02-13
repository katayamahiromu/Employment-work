#include"RiverSoundSystem.h"
#include"Graphics/GraphicsManager.h"
#include"math/Mathf.h"
#include"Audio/AudioManager.h"
#include"../Macro.h"

RiverSoundSystem::RiverSoundSystem()
{

}

RiverSoundSystem::~RiverSoundSystem()
{

}

void RiverSoundSystem::update()
{
	SoundListner lister = *AudioManager::instance()->findAudio(static_cast<int>(Lisner::PLAYER));
	CalcPos(lister.position);
}

void RiverSoundSystem::CalcPos(DirectX::XMFLOAT3 listenerPos)
{
	float minDistSq = FLT_MAX;
	DirectX::XMFLOAT3 bestPoint = SoundPos;

	for (size_t i = 1; i < pts.size() - 2; ++i)
	{
		for (int s = 0; s < SUBDIV; ++s)
		{
			float t0 = (float)s / SUBDIV;
			float t1 = (float)(s + 1) / SUBDIV;

			DirectX::XMFLOAT3 p0 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t0);
			DirectX::XMFLOAT3 p1 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t1);

			DirectX::XMFLOAT3 cp = Mathf::ClosestPointOnSegment(p0, p1, listenerPos);

			float distSq = Mathf::calcDistanceSqXYZ(cp, listenerPos);

			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				bestPoint = cp;
			}
		}
	}

	// 最近接点を保存
	SoundPos = bestPoint;
}

void RiverSoundSystem::debugRender()
{
	//簡易的なデバック用の線
	DirectX::XMFLOAT4 color = { 0.0f,0.0f,0.0f,0.0f };

	GraphicsManager* graphics = GraphicsManager::instance();

	for (size_t i = 1; i < pts.size() - 2; ++i)
	{
		for (int s = 0; s < SUBDIV; ++s)
		{
			float t0 = (float)s / SUBDIV;
			float t1 = (float)(s + 1) / SUBDIV;

			DirectX::XMFLOAT3 p0 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t0);
			DirectX::XMFLOAT3 p1 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t1);

			graphics->getLineRenderer()->addVertex(p0, color);
			graphics->getLineRenderer()->addVertex(p1, color);
		}
	}

	//球体
	graphics->getDebugRenderer()->drawSphere(SoundPos, 0.5f, { 1.0f,0.0f,0.0f,1.0f });
}

void RiverSoundSystem::gui()
{

}