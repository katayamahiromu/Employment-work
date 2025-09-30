#include"Particle.h"
#include"DeviceManager.h"

Particle::Particle(const wchar_t*filename)
{
	DeviceManager* deviceMgr = DeviceManager::instance();

	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	//	パーティクル用画像ロード
	TextureManager::instance()->loadTextureFromFile(deviceMgr->getDevice(), filename, shader_resource_view.GetAddressOf(), &texture2d_desc);

	//	パーティクルシステム生成
	particleSystem = std::make_unique<ParticleSystem>(deviceMgr->getDevice(), shader_resource_view, 5, 5);
}

void Particle::render(const DirectX::XMFLOAT4X4&view,const DirectX::XMFLOAT4X4&projection)
{
	if (particleSystem)
		particleSystem->Render(DeviceManager::instance()->getDeviceContext(), view, projection);
}