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
#include"Graphics/LuminanceExtract.h"
#include"Graphics/ColorGrading.h"
#include"Graphics/ACES_Filmic.h"
#include"Graphics/RewidLine.h"
#include"ResourceList/AuidoResourceList.h"
#include"math/Collision.h"
#include"imgui.h"
// ������
void GameScene::initialize()
{
	collisionSystem = std::make_unique<CollisionSystem>();

	DeviceManager* deviceMgr = DeviceManager::instance();
	objManager = std::make_unique<ObjectManager>();

	stageManager = std::make_unique<StageManager>(*objManager.get());

	playerManager = std::make_unique<PlayerManager>(*objManager.get());
	enemyManager = std::make_unique<EnemyManager>(*objManager.get());
	enemyManager->enemyCreate(playerManager->getPlayer());

	uiManager = std::make_unique<UIManager>();
	uiManager->registerUi(new RewindTimeUI(*playerManager->getPlayer()->GetComponent<TimeLapse>()));

	skymap = std::make_unique<Skymap>(L"Resources/Image/Sky.png");
	
	//�|�X�g�G�t�F�N�g
	PostprocessingRenderer* PostEffects = PostprocessingRenderer::instance();
	//PostEffects->addPostProcess(new LuminanceExtract);
	//PostEffects->addPostProcess(new ColorGrading);
	PostEffects->addPostProcess(new ACES_Filmic);

	//�J���������ݒ�
	Camera::instance()->defaultSetting();
	cameraCtrl = std::make_unique<CameraController>();

	//�p�[�e�B�N��
	particleMgr = std::make_unique<ParticleManager>();
	particleMgr->regist(new FireParticle);

	//�����̍Ō�̋L�^�ɕ��Q���o�邽�ߑS�Ẵg�����X�t�H�[���̍X�V
	objManager->updateTransform();

	AudioResourceList* list = AudioResourceList::instance();
	list->loadResource("Resources\\data\\SceneGameAudioData.csv");
	list->getAudio("bgm")->play(true);
}

// �I������
void GameScene::finalize()
{
	objManager->clear();
	//AudioResourceList::instance()->allClear();
}

// �X�V����
void GameScene::update(float elapsedTime)
{
	////�J�����̍X�V����
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(playerManager->getPlayer()->getTransform());
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front,M.r[2]);

	float yaw = atan2f(front.x, front.z);

	cameraCtrl->setFrontY(yaw);
	cameraCtrl->update(elapsedTime);

	objManager->update(elapsedTime);

	uiManager->update(elapsedTime);

	//�p�[�e�B�N���V�X�e���X�V
	particleMgr->update(elapsedTime);

	//�|�X�g�G�t�F�N�g�̍X�V
	PostprocessingRenderer::instance()->update(elapsedTime);

	collisionSystem->update();
}


// �`�揈��
void GameScene::render()
{
	GraphicsManager* graphics = GraphicsManager::instance();
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();
	PostprocessingRenderer* PostEffects = PostprocessingRenderer::instance();
	PostEffects->getPostProcess()->prepare(dc);
	//�X�J�C�}�b�v
	skymap->Render(dc, *camera);

	// 3D�f�o�b�O�`��
	{
		graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
			// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
			dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
			dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
			// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
			dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
			// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
			dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::MODEL)].Get());
			}
		);

		//�f�o�b�N�p�̃����_���[�\��
		collisionSystem->debugRender();

		// ���C�������_���`����s
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// �f�o�b�O�����_���`����s
		graphics->getDebugRenderer()->render(dc, *view, *proj);
	}

	// 3D �`��
	{
		DirectX::XMFLOAT4 cameraPos;
		cameraPos.x = camera->getEye()->x;
		cameraPos.y = camera->getEye()->y;
		cameraPos.z = camera->getEye()->z;
		cameraPos.w = 0.0f;

		// 3D �`��ɗ��p����萔�o�b�t�@�̍X�V�Ɛݒ�
		objManager->render(*view, *proj, cameraPos);
	}

	//�p�[�e�B�N���`��
	particleMgr->render(*view, *proj);


	// 2D�f�o�b�OGUI�`��
	{
		cameraCtrl->OnGui();
		//AudioResourceList::instance()->onGui();
	}

	PostEffects->getPostProcess()->clean(dc);
	PostEffects->render();

	// 2D �`��
	uiManager->render(dc);
	PostEffects->debugGui();
}