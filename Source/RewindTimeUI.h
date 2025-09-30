#pragma once
#include"UI.h"
#include"Graphics/Sprite.h"
#include"Component/TimeLapse.h"
#include<memory>

class RewindTimeUI : public UI
{
public:
	RewindTimeUI(TimeLapse&tl);
	~RewindTimeUI()override;

	void update(float elapsedTime)override;
	void render(ID3D11DeviceContext* dc);
private:
	std::unique_ptr<Sprite>baseLine;
	std::unique_ptr<Sprite>gage;
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	float gageWidth = 0;
	TimeLapse& tl;
};