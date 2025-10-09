#pragma once

#include <wrl.h>
#include<Scene.h>
#include"CameraController.h"
#include"PlayerManager.h"
#include"StageManager.h"
#include"EnemyManager.h"
#include"UIManager.h"
#include"Graphics/Skymap.h"
#include"system/PostprocessingRenderer.h"
#include"system/CollisionSystem.h"

#include"particle/ParticleManager.h"

#include"Audio/AudioManager.h"

// �Q�[���V�[��
class GameScene : public Scene
{
public:
	GameScene() {}
	~GameScene() {}

	// ������
	void initialize()override;

	// �I������
	void finalize()override;

	// �X�V����
	void update(float elapsedTime)override;

	// �`�揈��
	void render()override;
private:
	std::unique_ptr<CameraController>cameraCtrl;
	std::unique_ptr<ObjectManager>objManager;
	std::unique_ptr<StageManager>stageManager;
	std::unique_ptr<PlayerManager>playerManager;
	std::unique_ptr<EnemyManager>enemyManager;
	std::unique_ptr<UIManager>uiManager;
	std::unique_ptr<ParticleManager>particleMgr;
	std::unique_ptr<CollisionSystem>collisionSystem;

	std::unique_ptr<Skymap>skymap;
};
