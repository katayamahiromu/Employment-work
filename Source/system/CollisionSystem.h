#pragma once
#include<DirectXMath.h>
#include<vector>
#include<map>
#include"Component/CollisionConponent.h"


class CollisionSystem
{
public:
	CollisionSystem();
	~CollisionSystem();

	void update();
	void onGui();
	void debugRender();
private:
	void registerCollisionComponent(void*data);

	std::vector<CollisionComponent*>componentArray;
	uint64_t REGISTERKEY;
};