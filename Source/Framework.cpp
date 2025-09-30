#include <memory>
#include <sstream>
#include <d3d11.h>

#include "Framework.h"
#include "TitleScene.h"
#include "Graphics/GraphicsManager.h"
#include "SceneManager.h"

// コンストラクタ
Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
	// デバイス管理の初期化
	deviceMgr = DeviceManager::instance()->initialize(hwnd);

	// 入力管理の初期化
	inputMgr = InputManager::instance()->initialize(hwnd);

	// オーディオ管理の初期化
	audioMgr = AudioManager::instance()->initialize();

	// グラフィックス管理の初期化
	GraphicsManager::instance()->initialize(deviceMgr->getDevice(), deviceMgr->getDeviceContext());

	// imgui の作成
	imguiRenderer = std::make_unique<ImGuiRenderer>(hwnd, deviceMgr->getDevice());
	
	// シーンの作成
	SceneManager::instance()->changeScene(new TitleScene);
}

// デストラクタ
Framework::~Framework()
{

}

// 更新処理
void Framework::update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	inputMgr->update();

	// シーン更新処理
	SceneManager::instance()->update(elapsedTime);
}

// 描画処理
void Framework::render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 排他処理
	std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

	ID3D11DeviceContext* dc = deviceMgr->getDeviceContext();

	// IMGUIフレーム開始処理
	imguiRenderer->newFrame();

	// シーン描画処理
	SceneManager::instance()->render();

	// demo
	//ImGui::ShowDemoWindow();

	// IMGUI描画
	imguiRenderer->render(dc);

	// バックバッファに描画した画を画面に表示する。
	deviceMgr->getSwapChain()->Present(syncInterval, 0);
}

// フレームレート計算
void Framework::calculateFrameStats()
{
	if (++frames, (timer.timeStamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::ostringstream outs;
		outs.precision(6);
		outs << " : FPS : " << fps << " / " << "Frame Time : " << 1000.0f / fps << " (ms)";
		SetWindowTextA(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

// アプリケーションループ
int Framework::run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();
			calculateFrameStats();

			float elapsedTime = syncInterval == 0 ? timer.timeInterval() : syncInterval / 60.0f;
			
			update(elapsedTime);
			render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (imguiRenderer->handleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.start();
		break;
	case WM_MOUSEWHEEL:
		InputManager::instance()->getMouse()->setWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
