#pragma once
#include"Component.h"
#include <functional>
enum CollisionType
{
	Mesh,
	None,
};

struct collisionSphereInfo
{
	DirectX::XMFLOAT3 pos;
	float size;
};

class CollisionComponent : public Component
{
public:
	CollisionComponent(CollisionType type);
	~CollisionComponent()override;

	const char* getName() const override { return "Collision Component"; }
	void prepare()override;
	void update(float elapsedTime)override;
	void OnGUI() override;

	void setMeshName(std::string name);
	void setBoneInfo(std::string name,float size);

	std::vector<collisionSphereInfo> &getSphereInfo() { return collisionSphereInfoArray; }
private:

	void meshCollisionSetting();
	void noneCollisionSetting();
	CollisionType type;
	std::vector<collisionSphereInfo>collisionSphereInfoArray;
	std::vector<int> meshIndexArray;
	std::vector<int>boneIndexArray;

	std::function<void()>func;
};