#pragma once
#include"PostProcessing.h"
#include<DirectXMath.h>

class LuminanceExtract : public PostProcess
{
public:
	LuminanceExtract();
	~LuminanceExtract();

	struct luminanceExtractConstants
	{
		float threshold{ 0.05f }; //高輝度抽出のための閾値
		float intensity{ 1.4f }; //ブルームの強度
		DirectX::XMFLOAT2 dummy{0.0f,0.0f};
	};
	void debugGui()override;
	void update(float elapsedTime)override;
	void bindShader()override;
private:
	luminanceExtractConstants luminanceExtractConstant;
};
