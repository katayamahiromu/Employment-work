#pragma once
#include"Component/object.h"
#include"StageManager.h"

class EnemyManager
{
public:
	EnemyManager(ObjectManager& om);
	~EnemyManager();

	void enemyCreate(Object*o);
private:
	std::list<std::shared_ptr<Object>>enemyArray;
	ObjectManager* om;
	StageManager* sm = nullptr;
};
