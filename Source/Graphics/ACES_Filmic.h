#pragma once

#include"PostProcessing.h"
#include<DirectXMath.h>

class ACES_Filmic : public PostProcess
{
public:
	ACES_Filmic();
	~ACES_Filmic() {};

	void debugGui()override;
};