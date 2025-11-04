#pragma once
#include<d3d11.h>
#include<wrl.h>

class MrtAttachment
{
public:
	MrtAttachment();
	~MrtAttachment();

	ID3D11RenderTargetView* getRtv() { return rtv.Get(); }
	ID3D11ShaderResourceView* getSrv() { return srv.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
};