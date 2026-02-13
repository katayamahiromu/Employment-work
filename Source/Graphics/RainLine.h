#pragma once
#include"PostProcessing.h"

class RainLine : public PostProcess
{
public:
	RainLine();
	~RainLine();

	void update(float elapsedTime)override;

	void debugGui()override;

	void bindShader()override;
private:
	struct RainParam
	{
		float rainIntensity = 0.5f; // 雨の強度
		float rainSpeed = 2.0f;		// 落下速度
		float rainScale = 12.0f;	// 雨の密度（UVスケール）
		float time = 0.0f;			//時間
	};

	RainParam param;
};