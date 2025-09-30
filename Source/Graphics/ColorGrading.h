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
		float hueShift = 0.0f;//F‘Š•â³
		float saturation = 1.2f;//Ê“x’²®
		float brightness = 1.0f;//–¾“x’²®
		float dummy = 0.0f;
	};
	void debugGui()override;
	void update(float elapsedTime)override;
private:
	CBColorGrading colorGrading;
};