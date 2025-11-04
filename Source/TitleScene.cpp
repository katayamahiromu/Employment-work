#include"TitleScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"Input/InputManager.h"
#include"SceneManager.h"
#include"GameScene.h"
#include"LoadingScene.h"
#include"imgui.h"
#include"modelScene.h"


//初期化処理
void TitleScene::initialize()
{
	//タイトル画像読み込み
	titleImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\Title.png");
	test.push("aaa");
	test.push("bbb");
	test.push("ccc");
	test.push("ddd");
	test.push("eee");

	//objManager = std::make_unique<ObjectManager>();
	//cmrController = std::make_unique<CameraController>();

	//DeviceManager* deviceMgr = DeviceManager::instance();
	//const float scale_fcator = 0.1f;	// モデルが大きいのでスケール調整
	//DirectX::XMFLOAT3 scale = { scale_fcator, scale_fcator, scale_fcator };
	//std::shared_ptr<Object>obj = objManager->create();
	//obj->loadModel("Resources\\Model\\player\\Chiyo School Dress.fbx");
	//obj->setName("test enemy");
	//obj->setScale(scale);
	//obj->setPosition({ 0.0f,0.0f,0.0f });

	////カメラ初期設定
	//Camera* camera = Camera::instance();
	//camera->setLookAt(
	//	DirectX::XMFLOAT3(0, 10, -10),// カメラの視点（位置）
	//	DirectX::XMFLOAT3(0, 0, 0),	  // カメラの注視点（ターゲット）
	//	DirectX::XMFLOAT3(0, 1, 0)    // カメラの上方向
	//);

	//camera->setPerspectiveFov(
	//	DirectX::XMConvertToRadians(45),	// 視野角
	//	deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// 画面縦横比率
	//	0.1f,		// カメラが映し出すの最近距離
	//	1000.0f		// カメラが映し出すの最遠距離
	//);

	//sample = AudioManager::instance()->loadAudioSource("Resources\\Audio\\グレート.wav");
	sample = AudioManager::instance()->loadAudioSource("Resources\\Audio\\04 checkpoint.wav");
	sample->play(true);

	spectrum = std::make_unique<Spectrum>(DeviceManager::instance()->getDevice(), 512);
}

//終了化
void TitleScene::finalize()
{
	//objManager->clear();
}

//更新処理
void TitleScene::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	//ボタンを押したらゲームシーンに遷移
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
	}

	spectrum->update(sample.get());

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
	if (ImGui::Button("Push"))		//sendボタンが押されたら
	{
		if (strcmp(text, "") != 0)//入力ウィンドウに入力がある
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


	if (ImGui::Button("model scene"))
	{
		SceneManager::instance()->changeScene(new ModelScene);
	}

	float playTime = sample->getPlayTime();
	ImGui::InputFloat("play time", &playTime);
	ImGui::End();

	ImGui::Begin("Spectrum");
	std::vector<float>data = *spectrum->getSpectrumData();
	ImGui::PlotLines("FFT",data.data(), (int)data.size(), 0, nullptr, 0.0f, 1.0f, ImVec2(0, 150));
	spectrum->OnGUi();
	ImGui::End();
}

//描画処理
void TitleScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	//画面クリア＆レンダーターゲットビュー設定
	FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	
	// 2D 描画設定
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

	// 2D 描画
	{
		titleImage->render(dc,
			0, 0, mgr->getScreenWidth(), mgr->getScreenHeight(),
			1, 1, 1, 1,
			0,
			0, 0, 1280, 720
		);
	}


	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	
	spectrum->draw(dc);

	Gui();
}