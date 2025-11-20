#include"PostprocessingRenderer.h"
#include"Graphics/GraphicsManager.h"
#include"DeviceManager.h"
#include"imgui.h"

PostprocessingRenderer::PostprocessingRenderer()
{
	fullScreenQuad = std::make_unique<FullScreenQuad>(DeviceManager::instance()->getDevice());
	scenePostProcess = std::make_unique<PostProcess>();
	PostprocessingRendererManager::instance()->add(this);
}

PostprocessingRenderer::~PostprocessingRenderer()
{
	for (auto& p : postProcessArray)delete p;
	PostprocessingRendererManager::instance()->remove(this);
}

void PostprocessingRenderer::remove(PostProcess*pp)
{
	std::erase(postProcessArray, pp);
}

void PostprocessingRenderer::update(float elapsedTime)
{
	for (auto& p : postProcessArray)p->update(elapsedTime);
}

void PostprocessingRenderer::clear()
{
	for (auto& p : postProcessArray)delete p;
	//サイズの初期化
	postProcessArray.resize(0);
}


void PostprocessingRenderer::execution()
{
	GraphicsManager* graphics = GraphicsManager::instance();
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();

	//描画設定
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

	//ポストエフェクトでの書き込み
	PostProcess* cache = scenePostProcess.get();

	for (auto& p : postProcessArray)
	{
		//それぞれのパスに書き込み
		p->prepare(dc);
		p->bindShader();
		fullScreenQuad->bilt(dc, cache->getSrv(), 0, 1, p->getPixelShader());
		p->unBind();
		p->clean(dc);
		cache = p;
	}

	cacheSrv = cache->getSrvP();
}

void PostprocessingRenderer::render()
{
	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	DeviceManager::instance()->settingRender();
	fullScreenQuad->bilt(dc, &cacheSrv, 0, 1);
}

void PostprocessingRenderer::debugGui()
{
	if (ImGui::TreeNode("Scene"))
	{
		ImGui::Image(scenePostProcess->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
	ImGui::Separator();
	
	for (auto& p : postProcessArray)p->debugGui();
}

void PostprocessingRendererManager::Gui()
{
	ImGui::Begin("PostprocessingRenderer");

	int id = 0;
	for (auto& pr : PostEffectArray)
	{
		ImGui::PushID(id++);
		if (ImGui::TreeNode("Renderer"))
		{
			pr->debugGui();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	ImGui::End();
}