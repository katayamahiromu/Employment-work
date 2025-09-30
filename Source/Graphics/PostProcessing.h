#pragma once
#include"Graphics/FrameBuffers.h"
#include<memory>

class PostProcess
{
public:
	PostProcess();
	~PostProcess() {};
	virtual void debugGui() {};
	virtual void update(float elapsedTime) {};
	void prepare(ID3D11DeviceContext* dc);
	void clean(ID3D11DeviceContext* dc);

	ID3D11ShaderResourceView* getSrvP() { return framebuffer.get()->getSrvP(); }
	ID3D11ShaderResourceView** getSrv() {return framebuffer.get()->getSrv(); }
	ID3D11PixelShader* getPixelShader() { return pixelShader.Get(); }
protected:
	std::unique_ptr<FrameBuffer>framebuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>pixelShader;
};