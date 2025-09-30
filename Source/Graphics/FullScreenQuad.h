#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<cstdint>

//ポストエフェクト用のスプライト
class FullScreenQuad
{
public:
	FullScreenQuad(ID3D11Device* dc);
	virtual ~FullScreenQuad() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>embeddedPixelShader;
public:
	void bilt(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
		uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader = nullptr);
};