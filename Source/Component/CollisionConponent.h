#pragma once
#include"Component.h"

enum CollisionType
{
	Mesh,
	None,
};

class CollisionComponent : public Component
{
public:
	CollisionComponent(CollisionType type);
	~CollisionComponent()override;

	const char* getName() const override { return "Collision Component"; }
	void prepare()override;
	void update(float elapsedTime)override;

	void setMeshName(std::string name);
	void setBoneName(std::string name);

	std::vector<DirectX::XMFLOAT3>getPositionArray() { return collisionPos; }
private:

	void meshCollisionSetting();
	void noneCollisionSetting();
	CollisionType type;
	std::vector<DirectX::XMFLOAT3>collisionPos;
	std::vector<int> meshIndexArray;
	std::vector<int> boneIndexArray;
};