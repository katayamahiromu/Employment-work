#include"WeaponManager.h"

WeaponManager::WeaponManager(ObjectManager& om, Object& obj)
	:om(&om),owner(&obj)
{

}

WeaponManager::~WeaponManager()
{

}

void WeaponManager::create()
{
	std::shared_ptr<Object>obj = om->create();
	obj->loadModel("Resources\\Model\\Nekobuki\\NekodaggerA.fbx");
	obj->setName("Dagger");
	const float scale_fcator = 0.05f;
	DirectX::XMFLOAT3 scale = { scale_fcator, scale_fcator, scale_fcator };
	obj->setScale(scale);
	obj->AddComponent<Weapon>(*owner);
	weaponArray.emplace_back(obj);
}