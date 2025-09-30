#include"UIManager.h"
#include"Graphics/GraphicsManager.h"

UIManager::UIManager()
{

}

UIManager::~UIManager()
{
	clear();
}

void UIManager::update(float elapsedTime)
{
	for (UI* ui : UIArray)
	{
		ui->update(elapsedTime);
	}

	for (UI* ui : removeArray)
	{
		std::list<UI*>::iterator it = std::find(UIArray.begin(), UIArray.end(), ui);
		if (it != UIArray.end())
		{
			UIArray.erase(it);
		}
		delete ui;
	}

	removeArray.clear();
}

void UIManager::render(ID3D11DeviceContext* dc)
{
	GraphicsManager* graphics = GraphicsManager::instance();
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

	for (UI* ui : UIArray)
	{
		ui->render(dc);
	}
}

void UIManager::clear()
{
	for (UI* ui : UIArray)
	{
		delete ui;
	}
	UIArray.clear();
}