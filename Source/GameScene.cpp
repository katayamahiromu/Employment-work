#include "GameScene.h"
#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Component/object.h"
#include "Camera.h"

#include "TitleScene.h"
#include "SceneManager.h"
#include "Input/InputManager.h"
#include "math/Mathf.h"

#include"RewindTimeUI.h"
#include"HPBar.h"

#include"Graphics/LuminanceExtract.h"
#include"Graphics/GaussianFiltering.h"
#include"Graphics/ColorGrading.h"
#include"Graphics/ACES_Filmic.h"
#include"Graphics/BlockToAlpha.h"

#include"system/TimeLapseManager.h"
#include"../Source/ResourceList/AuidoResourceList.h"

#include"imgui.h"
// 初期化
void GameScene::initialize()
{
	//カメラ初期設定
	cameraCtrl = std::make_unique<CameraController>();

	collisionSystem = std::make_unique<CollisionSystem>();

	DeviceManager* deviceMgr = DeviceManager::instance();

	//オブジェクト設定
	objManager = std::make_unique<ObjectManager>();

	stageManager = std::make_unique<StageManager>(*objManager.get());

	playerManager = std::make_unique<PlayerManager>(*objManager.get());
	playerManager->playerCreate(cameraCtrl->getCamera());

	enemyManager = std::make_unique<EnemyManager>(*objManager.get());
	enemyManager->enemyCreate(playerManager->getPlayer());

	uiManager = std::make_unique<UIManager>();
	uiManager->registerUi(new RewindTimeUI(*playerManager->getPlayer()->GetComponent<TimeLapse>()));
	//uiManager->registerUi(new HPBar(&Hp));

	skymap = std::make_unique<Skymap>(L"Resources/Image/cuve.png");
	
	//ポストエフェクト
	//PostprocessingRenderer* PostEffects = PostprocessingRenderer::instance();
	PostEffects = std::make_unique<PostprocessingRenderer>();
	PostEffects->addPostProcess(new LuminanceExtract);
	PostEffects->addPostProcess(new GaussianFilter);
	PostEffects->addPostProcess(new ColorGrading);
	PostEffects->addPostProcess(new ACES_Filmic);

	uiPostEffects = std::make_unique<PostprocessingRenderer>();
	uiPostEffects->addPostProcess(new LuminanceExtract);
	//uiPostEffects->addPostProcess(new GaussianFilter);
	uiPostEffects->addPostProcess(new ColorGrading);
	uiPostEffects->addPostProcess(new BlackToAlpha);

	//パーティクル
	particleMgr = std::make_unique<ParticleManager>();
	particleMgr->regist(new FireParticle);

	//処理の最後の記録に弊害が出るため全てのトランスフォームの更新
	objManager->updateTransform();

	AudioResourceList* list = AudioResourceList::instance();
	list->loadResource("Resources\\data\\SceneGameAudioData.csv",true);
	//list->getAudio("bgm")->play(true);

	//シーン描画
	scene = std::make_unique<Sprite>(deviceMgr->getDevice(), nullptr);
	scene->resizeTextureSize(deviceMgr->getDevice(), 1280, 720);

	//uiシーン
	uiScene = std::make_unique<Sprite>(deviceMgr->getDevice(), nullptr);
	uiScene->resizeTextureSize(deviceMgr->getDevice(), 1280, 720);

	AudioResourceList::instance()->getAudio("bgm")->play(true);
}

// 終了処理
void GameScene::finalize()
{
	objManager->clear();
	//AudioResourceList::instance()->allClear();
}

// 更新処理
void GameScene::update(float elapsedTime)
{
	//カメラの更新処理
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(playerManager->getPlayer()->getTransform());
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front,M.r[2]);

	float yaw = atan2f(front.x, front.z);

	cameraCtrl->setFrontY(yaw);
	cameraCtrl->update(elapsedTime);

	//オブジェクト更新
	objManager->update(elapsedTime);

	//UI更新
	uiManager->update(elapsedTime);

	//パーティクルシステム更新
	particleMgr->update(elapsedTime);

	//コリジョンシステムの更新
	collisionSystem->update();

	//タイムラプスの更新
	TimeLapseManager::instance().update();

	//ポストエフェクトの更新
	PostEffects->update(elapsedTime);
	uiPostEffects->update(elapsedTime);
}


void GameScene::sceneRender()
{
	GraphicsManager* graphics = GraphicsManager::instance();
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();

	Camera* camera = cameraCtrl->getCamera();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	PostEffects->getPostProcess()->prepare(dc);
	//スカイマップ
	skymap->Render(dc, *camera);

	// 3Dデバッグ描画
	{
		graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
			// サンプラーステートの設定（アニソトロピック）
			dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			// ブレンドステートの設定（アルファ）
			dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
			// 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
			dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
			// ラスタライザステートの設定（ソリッド、裏面表示オフ）
			dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::MODEL)].Get());
			}
		);

		//デバック用のレンダラー表示
		collisionSystem->debugRender();

		// ラインレンダラ描画実行
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// デバッグレンダラ描画実行
		graphics->getDebugRenderer()->render(dc, *view, *proj);
	}

	// 3D 描画
	{
		DirectX::XMFLOAT4 cameraPos;
		cameraPos.x = camera->getEye()->x;
		cameraPos.y = camera->getEye()->y;
		cameraPos.z = camera->getEye()->z;
		cameraPos.w = 0.0f;

		// 3D 描画に利用する定数バッファの更新と設定
		objManager->render(*view, *proj, cameraPos);
	}

	//パーティクル描画
	particleMgr->render(*view, *proj);


	// 2DデバッグGUI描画
	{
		cameraCtrl->OnGui();
		//AudioResourceList::instance()->onGui();
	}

	PostEffects->getPostProcess()->clean(dc);

	PostEffects->execution();
}

void GameScene::UIRender()
{
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	uiPostEffects->getPostProcess()->prepare(dc);
	uiManager->render(dc);
	uiPostEffects->getPostProcess()->clean(dc);
	uiPostEffects->execution();
}

// 描画処理
void GameScene::render()
{

	sceneRender();
	UIRender();

	GraphicsManager* graphics = GraphicsManager::instance();
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	// 2D 描画
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	
	DeviceManager::instance()->settingRender();
	scene->setShaderResourceView(PostEffects->getCacheSrv());
	//scene->render(dc, { posX,posY,1280.0 / 2,720 / 2 }, { 1,1,1,1 });
	scene->render(dc, {0.0f,0.0f,1280.0f,720.0f}, { 1,1,1,1 });

	uiScene->setShaderResourceView(uiPostEffects->getCacheSrv());
	//uiScene->render(dc, { 1280.0 / 2, 0.0f ,1280.0 / 2,720 / 2 }, { 1,1,1,1 });
	uiScene->render(dc, { 0.0f,0.0f,1280.0f,720.0f }, { 1,1,1,1 });

	PostprocessingRendererManager::instance()->Gui();
}