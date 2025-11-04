#include"depthStencilTex.h"
#include"DeviceManager.h"
#include"Shader.h"
#include"imgui.h"

DepthStencilTex::DepthStencilTex()
{
	//深度用バッファテクスチャ
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = 1280;
	depthDesc.Height = 720;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Device* device = DeviceManager::instance()->getDevice();
	device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTex);

	//DSVの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(depthStencilTex.Get(), &dsvDesc, &depthStencilView);

	//srvの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(depthStencilTex.Get(), &srvDesc, &srv);

	//シェーダーの読み込み
	ShaderManager::instance()->createPsFromCso(device, "Shader//DepthPS.cso", pixelShader.GetAddressOf());
	framebuffer = std::make_unique<FrameBuffer>(device, 1280, 720);
}

DepthStencilTex::~DepthStencilTex()
{

}

void DepthStencilTex::debugGui()
{
	if (ImGui::TreeNode("Depth Buffer"))
	{
		ImGui::Image(framebuffer->getSrvP(), { 128, 128 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::TreePop();
	}
}

void DepthStencilTex::update(float elapsedTime)
{

}