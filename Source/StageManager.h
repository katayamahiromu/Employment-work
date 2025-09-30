#pragma once
#include<list>
#include"math/Collision.h"
#include"Component/object.h"

class StageManager
{
public:
	StageManager(ObjectManager&om);
	~StageManager();
	
	void CreateMainStage();
	//レイキャスト
	bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void rayCastRequest(void* data);
private:
	std::list<std::shared_ptr<Object>>stageArray;
	ObjectManager* om;

	uint64_t RAY_CAST_REQUEST_KEY;
};