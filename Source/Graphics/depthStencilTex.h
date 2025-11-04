#pragma once
#include"Graphics/PostProcessing.h"

class DepthStencilTex : public PostProcess
{
public:
	DepthStencilTex();
	~DepthStencilTex();

	void debugGui()override;
	void update(float elapsedTime)override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};