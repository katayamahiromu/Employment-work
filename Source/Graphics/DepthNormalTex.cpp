#include"DepthNormalTex.h"
#include"imgui.h"
#include"DeviceManager.h"

DepthNormalTex::DepthNormalTex()
{
	mrt = std::make_unique<MrtAttachment>();

	D3D11_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[1].BlendEnable = FALSE;
	blend_desc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[1].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device* device = DeviceManager::instance()->getDevice();

	device->CreateBlendState(&blend_desc, blendState.GetAddressOf());
}

DepthNormalTex::~DepthNormalTex()
{

}

void DepthNormalTex::Gui()
{
	ImGui::Begin("MRT");
	if (ImGui::TreeNode("DepthNormal"))
	{
		ImGui::Image(mrt->getSrv(), {128, 128}, {0, 0}, {1, 1}, {1, 1, 1, 1});
	}
	ImGui::End();
}