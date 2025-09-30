#include"PlayerManager.h"
#include"Player.h"
#include"StageManager.h"

PlayerManager::PlayerManager(ObjectManager& om):om(&om)
{
	//�}�l�[�W���[�������Ɉ�U��̃v���C���[���쐬
	playerCreate();
	weaponMgr = std::make_unique<WeaponManager>(om,*player.get());
	attachmentCreate();
}

PlayerManager::~PlayerManager()
{
	player.reset();
}

void PlayerManager::playerCreate()
{
	player = om->create();
	player->setName("Player");
	const float scale_factor = 0.01f;	// ���f�����傫���̂ŃX�P�[������
	DirectX::XMFLOAT3 scale = { scale_factor, scale_factor, scale_factor };
	player->setScale(scale);
	player->setHeight(1.2f);
	player->setRange(0.5f);
	player->loadModel("Resources\\Model\\nico\\nico.fbx");
	player->AddComponent<Player>();
	player->AddComponent<PlayerController>();
	player->AddComponent<Movement>();
	player->AddComponent<Animation>();
	player->AddComponent<TimeLapse>();
	player->AddComponent<Audio3DListener>(static_cast<int>(Lisner::PLAYER));
}

void PlayerManager::attachmentCreate()
{
	weaponMgr->create();
}