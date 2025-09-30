#pragma once

#include"PostProcessing.h"
#include<DirectXMath.h>

class RewindLine : public PostProcess
{
public:
	RewindLine();
	~RewindLine();

	void debugGui()override;
	void update(float elapsedTime)override;
private:
	struct RewindLineParameter
	{
		float time = 0.0f;
		DirectX::XMFLOAT2 screenSize{1280,720};
		float dummy = 0.0f;
	};

	RewindLineParameter rlParam;
};