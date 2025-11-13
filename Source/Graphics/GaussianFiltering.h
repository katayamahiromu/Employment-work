#pragma once
#include"PostProcessing.h"
#include<DirectXMath.h>

class GaussianFilter : public PostProcess
{
public:
	GaussianFilter();
	~GaussianFilter();

	void debugGui()override;
	void update(float elapsedTime)override;

	void bindShader()override;

	#define KERNEL_MAX 25
	struct Gaussian
	{
		DirectX::XMFLOAT4 weights[KERNEL_MAX * KERNEL_MAX];
		float kernelSize;
		DirectX::XMFLOAT2 texcel;
		float dummy;
	};

	struct GaussianDates
	{
		int kernelSize{ 9 };
		float sigma{ 10.0f };
		DirectX::XMFLOAT2 textureSize;
	};

private:
	void calcGaussianFilterConstant(Gaussian& constant,const GaussianDates&data);
private:
	Gaussian gaussian;
	GaussianDates data;
};