#include"TitleScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"SceneManager.h"
#include"GameScene.h"
#include"LoadingScene.h"
#include"imgui.h"
#include"modelScene.h"

#include"system/PostprocessingRenderer.h"
#include"Graphics/LuminanceExtract.h"
#include"Graphics/GaussianFiltering.h"
#include"Graphics/ColorGrading.h"
#include"Graphics/ACES_Filmic.h"
#include"Graphics/Mask.h"

#include<algorithm>

//初期化処理
void TitleScene::initialize()
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	//タイトル画像読み込み
	titleImage = std::make_unique<Sprite>(device, L"Resources\\Image\\Title.png");

	//2D設定
	rogo = std::make_unique<Button>(L"Resources\\Image\\title2.png");
	rogo->setPos({ 600,-50 });
	rogo->setSize({ 600,600 });

	Button* start = new Button(L"Resources\\Image\\button1.png");
	start->loadReplaceSprite(L"Resources\\Image\\button1_replace.png");
	start->setPos({ 270,460 });
	start->setSize({ 260,100 });
	choices.push_back(start);

	Button* exit = new Button(L"Resources\\Image\\button2.png");
	exit->loadReplaceSprite(L"Resources\\Image\\button2_replace.png");
	exit->setPos({ 710,460 });
	exit->setSize({ 260,100 });
	choices.push_back(exit);

	titleImage = std::make_unique<Sprite>(device, L"Resources/Image/title_back.png");

	//3D設定
	objManager = std::make_unique<ObjectManager>();
	objManager->setLineSize(0.008f);

	//カメラ初期設定
	fixCamera = std::make_unique<Camera>();
	DirectX::XMFLOAT3 eye{ -4.426f, 0.492f, -0.476f };
	DirectX::XMFLOAT3 forcus{ 4.426f,-0.492f,-2.381f };
	DirectX::XMFLOAT3 up{ -1.475f,4.426f,0.476f };
	fixCamera->setLookAt(eye,forcus,up);

	model = objManager->create();
	model->loadModel("Resources\\Model\\pico\\pico_chan_chr_pico_00.fbx");

	const float scale_factor = 0.01f;	// モデルが大きいのでスケール調整
	DirectX::XMFLOAT3 scale = { scale_factor, scale_factor, scale_factor };
	model->setScale(scale);

	DirectX::XMFLOAT3 direction = { -360.0f,0.0f,-51.0f };
	model->setRotation(direction);

	DirectX::XMFLOAT4 rotation{ 0.0f,-1.0,0.0f,-0.3f };
	model->setRotation(rotation);
	model->AddComponent<Animation>();
	model->AddComponent<PlayerController>();

	animation = model->GetComponent<Animation>();
	controller = model->GetComponent<PlayerController>();

	//ポストエフェクトの設定
	PostprocessingRenderer* PostEffects = PostprocessingRenderer::instance();
	PostEffects->addPostProcess(new LuminanceExtract);
	PostEffects->addPostProcess(new GaussianFilter);
	PostEffects->addPostProcess(new ColorGrading);
	PostEffects->addPostProcess(new ACES_Filmic);
	
	//idolのアニメーションを再生
	animation->playAnimation(1, true);
	//sample = AudioManager::instance()->loadAudioSource("Resources\\Audio\\グレート.wav");
	sample = AudioManager::instance()->loadAudioSource("Resources\\Audio\\04 checkpoint.wav");
	//sample->play(true);

	spectrum = std::make_unique<Spectrum>(DeviceManager::instance()->getDevice(), 512);

	//ボタン設定
	controller->registerFunc([&]()
		{
			//ボタンを押したらゲームシーンに遷移
			switch (select)
			{
			case Select::Start:SceneManager::instance()->changeScene(new LoadingScene(new GameScene));break;
			case Select::Exit:SceneManager::instance()->changeScene(nullptr);break;
			}
		},
		PlayerController::keyAllocation::key_A);
}

//終了化
void TitleScene::finalize()
{
	for (auto b : choices)delete b;
	PostprocessingRenderer::instance()->clear();
}

//更新処理
void TitleScene::update(float elapsedTime)
{
	//spectrum->update(sample.get());

	DirectX::XMFLOAT3 direction = { -360.0f,0.0f,-51.0f };
	model->setRotation(direction);
	objManager->update(elapsedTime);


	//セレクト
	updateSelector(elapsedTime);

	//選択されいる物だけ色を変える
	for (auto b : choices)b->replaceFlagOff();
	choices.at(select)->replaceFlagOn();

	PostprocessingRenderer::instance()->update(elapsedTime);
}

void TitleScene::Gui()
{
	ImGui::Begin("test");

	if (ImGui::Button("model scene"))
	{
		SceneManager::instance()->changeScene(new ModelScene);
	}
	ImGui::SliderFloat("animation rate", &rate,0.0f,1.0f);
	animation->setAnimationRate(rate);
	ImGui::InputInt("selct", &select);

	if (ImGui::Button("sample play"))
	{
		sample->DCPlay();
	}

	ImGui::SliderFloat("wetLevel", &wetLevel, 0.0f, 1.0f);
	ImGui::SliderFloat("roomSize", &roomSize, 0.0f, 1.0f);
	ImGui::SliderFloat("decayTime", &decayTime, 0.0f, 20.0f);

	/*SubMixVoiceManager*smv = AudioManager::instance()->getSmv();
	SoundEffect* SE1 = smv->getSubMixVoice(0)->getEffect(REVERB);
	static_cast<Reverb*>(SE1)->setDecayTime(decayTime);
	static_cast<Reverb*>(SE1)->setRoomSize(roomSize);
	static_cast<Reverb*>(SE1)->setWetLevel(wetLevel);

	ImGui::SliderFloat("delay", &delay, 0.0f, 1.0f);
	ImGui::SliderFloat("feedback", &feedback, 0.0f, 1.0f);
	ImGui::SliderFloat("wetDryMix", &wetDryMix, 0.0f, 1.0f);
	SoundEffect* SE2 = smv->getSubMixVoice(0)->getEffect(ECHO);
	static_cast<Echo*>(SE2)->setDelay(delay);
	static_cast<Echo*>(SE2)->setFeedback(feedback);
	static_cast<Echo*>(SE2)->setWetDryMix(wetDryMix);*/
	
	int count = AudioManager::instance()->PlayAudioCount();
	ImGui::InputInt("Audio Play Count", &count);
	ImGui::End();

	/*ImGui::Begin("Spectrum");
	std::vector<float>data = *spectrum->getSpectrumData();
	ImGui::PlotLines("FFT",data.data(), (int)data.size(), 0, nullptr, 0.0f, 1.0f, ImVec2(0, 150));
	spectrum->OnGUi();
	ImGui::End();*/
}

//描画処理
void TitleScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	//ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	//ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	////画面クリア＆レンダーターゲットビュー設定
	//FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	//dc->ClearRenderTargetView(rtv, color);
	//dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//dc->OMSetRenderTargets(1, &rtv, dsv);

	PostprocessingRenderer* PostEffects = PostprocessingRenderer::instance();
	PostEffects->getPostProcess()->prepare(dc);

	// 2D背景
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
	titleImage->render(dc, 0, 0, 1280, 720);

	//3D描画
	Camera* camera = fixCamera.get();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	DirectX::XMFLOAT4 cameraPos;
	cameraPos.x = camera->getEye()->x;
	cameraPos.y = camera->getEye()->y;
	cameraPos.z = camera->getEye()->z;
	cameraPos.w = 0.0f;

	objManager->render(*view, *proj, cameraPos);
	PostEffects->getPostProcess()->clean(dc);
	PostEffects->render();

	// 2D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ADD)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D 描画
	{
		rogo->draw();
		for (auto b : choices) b->draw();
	}

	//graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
	//	// サンプラーステートの設定（リニア）
	//	dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
	//	// ブレンドステートの設定（アルファ）
	//	dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);
	//	// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
	//	dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
	//	// ラスタライザステートの設定（ソリッド、裏面表示オフ）
	//	dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	//	});
	//
	//spectrum->draw(dc);

	Gui();
	//PostEffects->debugGui();
}

void TitleScene::updateSelector(float elapsedTime)
{
	paddingTime -= elapsedTime;
	if (paddingTime > 0.0f) return;

	DirectX::XMFLOAT2 axis = controller->getLeftStick();
	select += static_cast<int>(axis.x);
	select = std::clamp(select, static_cast<int>(Select::Start), static_cast<int>(Select::Exit));
	paddingTime = freezeTime;
}