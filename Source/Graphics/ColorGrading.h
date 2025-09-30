#pragma once
#include"PostProcessing.h"
#include<DirectXMath.h>

class ColorGrading : public PostProcess
{
public:
	ColorGrading();
	~ColorGrading();

	struct CBColorGrading
	{
		float hueShift = 0.0f;//�F���␳
		float saturation = 1.2f;//�ʓx����
		float brightness = 1.0f;//���x����
		float dummy = 0.0f;
	};
	void debugGui()override;
	void update(float elapsedTime)override;
private:
	CBColorGrading colorGrading;
};