#pragma once
#include"Weapon.h"
#include"Component/object.h"
#include<list>

class WeaponManager
{
public:
	WeaponManager(ObjectManager&om,Object&obj);
	~WeaponManager();

	void create();
private:
	ObjectManager* om;
	Object* owner;
	std::list<std::shared_ptr<Object>>weaponArray;
};