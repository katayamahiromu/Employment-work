#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<cstdint>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~FrameBuffer() = default;

	void clear(ID3D11DeviceContext* dc,
		float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
	void activate(ID3D11DeviceContext* dc);
	void deactivate(ID3D11DeviceContext* dc);
	
	ID3D11ShaderResourceView* getSrvP() { return shaderResourceViews[0].Get(); }
	ID3D11ShaderResourceView** getSrv() { return shaderResourceViews[0].GetAddressOf(); }
private:

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>shaderResourceViews[2];
	D3D11_VIEWPORT viewPort;

	UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};