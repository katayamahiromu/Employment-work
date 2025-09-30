#pragma once

#include"Component/Component.h"
#include"math/Collision.h"

//ステージ
class Stage : public Component
{
public:
	Stage();
	~Stage()override;

	const char* getName() const override { return "stage"; }

	void prepare() override;

	void update(float elapsedTime)override;
	bool  raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
};