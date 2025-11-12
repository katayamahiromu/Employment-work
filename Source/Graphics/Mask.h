#pragma once
#include"PostProcessing.h"
#include"Sprite.h"
#include<DirectXMath.h>

class Mask :public PostProcess
{
public:
	Mask();
	~Mask();

	void debugGui()override;
	void update(float elaspedTime)override;

	void bindShader()override;

	void unBind()override;
	struct MaskData
	{
		float dissolveThreshold = 1.0f;
		float edgeThreshold = 0.2f; //—Î‚Ìè‡’l
		float time = 0.0f; //ŠÔ
		float flickerFreq = 12.0f; //‰Š‚ä‚ç‚¬ü”g”

		DirectX::XMFLOAT4 innerColor = { 1.0f, 0.8f, 0.3f, 1.0f };
		DirectX::XMFLOAT4 outerColor = { 1.0f, 0.2f, 0.0f, 1.0f };
	};
private:
	MaskData data;
	std::unique_ptr<Sprite>mask;
	std::unique_ptr<Sprite>noise;
};