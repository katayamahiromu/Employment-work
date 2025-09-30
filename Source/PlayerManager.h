#pragma once
#include"Component/object.h"
#include"StageManager.h"
#include"WeaponManager.h"

class PlayerManager
{
public:
	PlayerManager(ObjectManager& om);
	~PlayerManager();

	void playerCreate();
	void attachmentCreate();
	Object* getPlayer() { return player.get(); }
private:
	std::shared_ptr<Object>player;
	std::unique_ptr<WeaponManager>weaponMgr;

	ObjectManager* om;
	StageManager* sm;
};