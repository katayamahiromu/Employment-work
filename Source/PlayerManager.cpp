#include"PlayerManager.h"
#include"Player.h"
#include"StageManager.h"

PlayerManager::PlayerManager(ObjectManager& om):om(&om)
{
	
}

PlayerManager::~PlayerManager()
{
	player.reset();
}

void PlayerManager::playerCreate(Camera* camera)
{
	if (!camera)return;

	player = om->create();
	player->setName("Player");
	const float scale_factor = 0.01f;	// モデルが大きいのでスケール調整
	DirectX::XMFLOAT3 scale = { scale_factor, scale_factor, scale_factor };
	player->setScale(scale);
	player->setHeight(1.2f);
	player->setRange(0.5f);
	player->loadModel("Resources\\Model\\pico\\pico_chan_chr_pico_00.fbx");
	player->AddComponent<Player>();
	player->AddComponent<PlayerController>();
	player->AddComponent<Movement>();
	player->AddComponent<Animation>();
	player->AddComponent<TimeLapse>();
	player->AddComponent<Audio3DListener>(static_cast<int>(Lisner::PLAYER));
	player->AddComponent<CollisionComponent>(CollisionType::Mesh);
	player->AddComponent<CameraInfo>(camera);

	attachmentCreate();
}

void PlayerManager::attachmentCreate()
{
	weaponMgr = std::make_unique<WeaponManager>(*om, *player.get());
	weaponMgr->create();
}