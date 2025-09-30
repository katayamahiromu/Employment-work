#include"stage.h"
#include"Component/object.h"

Stage::Stage()
{

}

Stage::~Stage()
{

}

void Stage::prepare()
{
	getObject()->setMaterialColor({ 0.8f,0.8f,0.8f,1.0f });
}

void Stage::update(float elapsedTime)
{

}

bool Stage::raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::intersectRayAndModel(start, end, getObject()->getModel(), hit);
}