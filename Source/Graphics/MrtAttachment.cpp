#include"MrtAttachment.h"
#include"DeviceManager.h"

MrtAttachment::MrtAttachment()
{
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = 1280;
	texture2d_desc.Height = 720;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;

	DXGI_FORMAT formats = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ID3D11Device* device = DeviceManager::instance()->getDevice();

	texture2d_desc.Format = formats;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>colorBuffer{};
	device->CreateTexture2D(&texture2d_desc, NULL, colorBuffer.GetAddressOf());

	//レンダーターゲット生成
	device->CreateRenderTargetView(colorBuffer.Get(), NULL, rtv.GetAddressOf());

	//シェーダーリソースビュー生成
	device->CreateShaderResourceView(colorBuffer.Get(), NULL, srv.GetAddressOf());
}

MrtAttachment::~MrtAttachment()
{

}