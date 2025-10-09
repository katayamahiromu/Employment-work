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

// ゲームシーン
class GameScene : public Scene
{
public:
	GameScene() {}
	~GameScene() {}

	// 初期化
	void initialize()override;

	// 終了処理
	void finalize()override;

	// 更新処理
	void update(float elapsedTime)override;

	// 描画処理
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
