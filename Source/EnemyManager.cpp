#include"EnemyManager.h"
#include"Enemy.h"

EnemyManager::EnemyManager(ObjectManager& om):om(&om)
{
	
}

EnemyManager::~EnemyManager()
{
	enemyArray.clear();
}

void EnemyManager::enemyCreate(Object*o)
{
	const float scale_fcator = 0.01f;	// モデルが大きいのでスケール調整
	DirectX::XMFLOAT3 scale = { scale_fcator, scale_fcator, scale_fcator };
	std::shared_ptr<Object>obj = om->create();
	obj->loadModel("Resources\\Model\\plantune\\plantune.fbx");
	obj->setName("test enemy");
	obj->setScale(scale);
	obj->setPosition({ 0.0f,0.0f,10.0f });

	obj->AddComponent<Enemy>(*o);
	obj->AddComponent<Movement>();
	obj->AddComponent<Animation>();
	obj->AddComponent<TimeLapse>();
	obj->AddComponent<Audio3DEmitter>(3.0f, 10.0f);
	obj->AddComponent<CollisionComponent>(CollisionType::Mesh);

	enemyArray.emplace_back(obj);
}