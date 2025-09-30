#pragma once
#include"Component/Collision.h"
#include<DirectXMath.h>
#include<vector>

class CollisionManager
{
public:
	CollisionManager() {};
	~CollisionManager() {};

	void update();
private:
	std::vector<Collision>collisionArray;
};