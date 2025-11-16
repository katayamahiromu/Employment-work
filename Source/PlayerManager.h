#pragma once
#include"Component/object.h"
#include"StageManager.h"
#include"WeaponManager.h"
#include"Camera.h"

class PlayerManager
{
public:
	PlayerManager(ObjectManager& om);
	~PlayerManager();

	void playerCreate(Camera* camera);
	void attachmentCreate();
	Object* getPlayer() { return player.get(); }
private:
	Camera* camera = nullptr;
	std::shared_ptr<Object>player;
	std::unique_ptr<WeaponManager>weaponMgr;

	ObjectManager* om;
	StageManager* sm;
};