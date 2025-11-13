#pragma once
#include"UI.h"
#include"Graphics/Sprite.h"
#include<memory>

class HPBar :public UI
{
public:
	HPBar(float* value);
	~HPBar();

	void update(float elapsedTime)override;
	void render(ID3D11DeviceContext* dc)override;
private:
	std::unique_ptr<Sprite>baseLine;
	std::unique_ptr<Sprite>gage;
	const float MaxGageWidth = 1072.0f;
	float gageWidth;

	float* manageValue;
	float initializeValue;
};