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

	struct Gaussian
	{
		DirectX::XMFLOAT2 texelSize;
		float offset = 1.0f;
		int sampleCount;

		struct Direction
		{
			DirectX::XMFLOAT2 dir;
			DirectX::XMFLOAT2 dummy;
		};

		Direction directions[8];
	};
private:
	void settingDirection();
private:
	Gaussian gaussian;
	int power = 1;
	int mode = 0;
};