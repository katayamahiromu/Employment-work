#include"PostprocessingRenderer.h"
#include"Graphics/GraphicsManager.h"
#include"DeviceManager.h"
#include"imgui.h"

PostprocessingRenderer::PostprocessingRenderer()
{
	fullScreenQuad = std::make_unique<FullScreenQuad>(DeviceManager::instance()->getDevice());
	scenePostProcess = std::make_unique<PostProcess>();
}

PostprocessingRenderer::~PostprocessingRenderer()
{
	for (auto& p : postProcessArray)delete p;
}

void PostprocessingRenderer::remove(PostProcess*pp)
{
	std::erase(postProcessArray, pp);
}

void PostprocessingRenderer::update(float elapsedTime)
{
	for (auto& p : postProcessArray)p->update(elapsedTime);
}

void PostprocessingRenderer::render()
{
	GraphicsManager* graphics = GraphicsManager::instance();
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

	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	
	PostProcess* cache = scenePostProcess.get();

	for (auto& p :postProcessArray)
	{
		//それぞれのパスに書き込み
		p->prepare(dc);
		fullScreenQuad->bilt(dc, cache->getSrv(), 0, 1,p->getPixelShader());
		p->clean(dc);
		cache = p;
	}

	//ワークバッファに戻す
	DeviceManager* mgr = DeviceManager::instance();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	fullScreenQuad->bilt(dc, cache->getSrv(), 0, 1);
}

void PostprocessingRenderer::debugGui()
{
	ImGui::Begin("Post Processing");

	if (ImGui::TreeNode("Scene"))
	{
		ImGui::Image(scenePostProcess->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
	
	for (auto& p : postProcessArray)p->debugGui();

	ImGui::End();
}