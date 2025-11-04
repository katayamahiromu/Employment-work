#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<memory>
#include"MrtAttachment.h"

class DepthNormalTex
{
public:
	DepthNormalTex();
	~DepthNormalTex();

	ID3D11RenderTargetView* getRtv() { return mrt->getRtv(); }
	ID3D11ShaderResourceView* getSrv() { return mrt->getSrv(); }

	void Gui();
private:
	std::unique_ptr<MrtAttachment>mrt;
	Microsoft::WRL::ComPtr<ID3D11BlendState>blendState;
};