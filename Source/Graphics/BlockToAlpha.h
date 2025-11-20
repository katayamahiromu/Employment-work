#pragma once
#include"PostProcessing.h"

class BlackToAlpha : public PostProcess
{
public:
	BlackToAlpha();
	~BlackToAlpha() {};

	void debugGui()override;
};