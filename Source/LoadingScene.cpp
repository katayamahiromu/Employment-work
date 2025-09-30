#include"LoadingScene.h"
#include"SceneManager.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

//初期化処理
void LoadingScene::initialize()
{
    //ローディング画像読み込み
	loadingImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\LoadingIcon.png");

    //別のスレッドの起動
    thread = new std::thread(LoadingThread, this);
}

//終了化処理
void LoadingScene::finalize()
{
    if (thread != nullptr)
    {
        thread->join(); //スレッドが自然に終了するのを待つ
        delete thread;
        thread = nullptr;
    }
}

//更新処理
void LoadingScene::update(float elapsedTime)
{
	constexpr float speed = 180.0f;
	angle += speed * elapsedTime;

    //次のシーンの初期化完了通知が完了していたらシーンを切り替える
    if (nextScene->isReady())
    {
        SceneManager::instance()->changeScene(nextScene);
        nextScene = nullptr;
    }
}

//描画処理
void LoadingScene::render()
{
    DeviceManager* mgr = DeviceManager::instance();
    GraphicsManager* graphics = GraphicsManager::instance();

    ID3D11DeviceContext* dc = mgr->getDeviceContext();
    ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
    ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
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
        // 画面の右下にローディング画像を表示
        float screenWidth = mgr->getScreenWidth();
        float screenHeight = mgr->getScreenHeight();
        float textureWidth = 256.0f;
        float textureHeight = 256.0f;
        float posX = screenWidth - textureWidth;
        float posY = screenHeight - textureHeight;

        loadingImage->render(dc,
            posX, posY, textureWidth, textureHeight,
            1, 1, 1, 1,
            angle,
            0, 0, textureWidth, textureHeight
        );
    }
}

void LoadingScene::LoadingThread(LoadingScene* scene)
{
    //DirectX関連のスレッドの初期化
    HREFTYPE hr = CoInitialize(nullptr);

    //次のシーンの初期化を行う
    scene->nextScene->initialize();

    //DirectX 関連のスレッドの終了化
    CoUninitialize();

    //次のシーンの準備完了設定
    scene->nextScene->setReady();
}