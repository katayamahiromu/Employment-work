#include"TitleScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"Input/InputManager.h"
#include"SceneManager.h"
#include"GameScene.h"
#include"LoadingScene.h"
#include"imgui.h"


//����������
void TitleScene::initialize()
{
	//�^�C�g���摜�ǂݍ���
	titleImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\Title.png");
	test.push("aaa");
	test.push("bbb");
	test.push("ccc");
	test.push("ddd");
	test.push("eee");

	//objManager = std::make_unique<ObjectManager>();
	//cmrController = std::make_unique<CameraController>();

	//DeviceManager* deviceMgr = DeviceManager::instance();
	//const float scale_fcator = 0.1f;	// ���f�����傫���̂ŃX�P�[������
	//DirectX::XMFLOAT3 scale = { scale_fcator, scale_fcator, scale_fcator };
	//std::shared_ptr<Object>obj = objManager->create();
	//obj->loadModel("Resources\\Model\\player\\Chiyo School Dress.fbx");
	//obj->setName("test enemy");
	//obj->setScale(scale);
	//obj->setPosition({ 0.0f,0.0f,0.0f });

	////�J���������ݒ�
	//Camera* camera = Camera::instance();
	//camera->setLookAt(
	//	DirectX::XMFLOAT3(0, 10, -10),// �J�����̎��_�i�ʒu�j
	//	DirectX::XMFLOAT3(0, 0, 0),	  // �J�����̒����_�i�^�[�Q�b�g�j
	//	DirectX::XMFLOAT3(0, 1, 0)    // �J�����̏����
	//);

	//camera->setPerspectiveFov(
	//	DirectX::XMConvertToRadians(45),	// ����p
	//	deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// ��ʏc���䗦
	//	0.1f,		// �J�������f���o���̍ŋߋ���
	//	1000.0f		// �J�������f���o���̍ŉ�����
	//);

	sample = AudioManager::instance()->loadAudioSource("Resources\\Audio\\04 checkpoint.wav");
	sample->play(true);

	sampleRevers = AudioManager::instance()->loadAudioSource("Resources\\Audio\\04 checkpoint.wav");
	sampleRevers->createReversWav();
}

//�I����
void TitleScene::finalize()
{
	//objManager->clear();
}

//�X�V����
void TitleScene::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	//�{�^������������Q�[���V�[���ɑJ��
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
	}

	/*objManager->update(elapsedTime);
	objManager->updateTransform();
	cmrController->debugUpdate(elapsedTime);*/
}

void TitleScene::Gui()
{
	ImGui::Begin("test");
	for (int i = 0;i < count;++i)
	{
		ImGui::Text(test.at(i).c_str());
	}
	ImGui::Separator();

	ImGui::InputText("", text, sizeof(text));
	ImGui::SameLine();
	if (ImGui::Button("Push"))		//send�{�^���������ꂽ��
	{
		if (strcmp(text, "") != 0)//���̓E�B���h�E�ɓ��͂�����
		{
			test.pushFront(text);
		}
		text[0] = '\0';

		if (count < 5) count++;
		count = std::clamp(count, 0, 5);
	}

	if (ImGui::Button("pop front"))
	{
		count--;
		test.frontAndPop();
	}
	ImGui::SameLine();
	if (ImGui::Button("pop back"))
	{
		count--;
		test.backAndPop();
	}


	if (ImGui::Button("revers"))
	{
		flag = !flag;
		flag ? sample->reversPlay(sampleRevers.get()) :sampleRevers->reversPlay(sample.get());
	}
	ImGui::End();
}

//�`�揈��
void TitleScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	//��ʃN���A�������_�[�^�[�Q�b�g�r���[�ݒ�
	FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	
	// 2D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i���j�A�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I�t�A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D �`��
	{
		titleImage->render(dc,
			0, 0, mgr->getScreenWidth(), mgr->getScreenHeight(),
			1, 1, 1, 1,
			0,
			0, 0, 1280, 720
		);
	}

	//Camera* camera = Camera::instance();
	//const DirectX::XMFLOAT4X4* view = camera->getView();
	//const DirectX::XMFLOAT4X4* proj = camera->getProjection();
	//{
	//	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
	//		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
	//		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
	//		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
	//		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
	//		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
	//		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
	//		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
	//		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::MODEL)].Get());
	//		}
	//	);

	//	// ���C�������_���`����s
	//	graphics->getLineRenderer()->render(dc, *view, *proj);

	//	// �f�o�b�O�����_���`����s
	//	graphics->getDebugRenderer()->render(dc, *view, *proj);
	//}

	//DirectX::XMFLOAT4 cameraPos;
	//cameraPos.x = camera->getEye()->x;
	//cameraPos.y = camera->getEye()->y;
	//cameraPos.z = camera->getEye()->z;
	//cameraPos.w = 0.0f;

	//objManager->render(*view, *proj, cameraPos);

	Gui();
}