#pragma once
#include"Audio/Audio3DSystem.h"

class RiverSoundSystem
{
public:
	RiverSoundSystem();
	~RiverSoundSystem();

	void update();

	void debugRender();

	void gui();

private:
	void CalcPos(DirectX::XMFLOAT3 linerPos);
private:
    // 制御点（川の中心ライン）
    std::vector<DirectX::XMFLOAT3> pts =
    {
        {  5, 1,0 },
        { 12, 1,15 },
        {  8, 1,30 },
        { 18, 1,45 },
        { 10, 1,60 },
        { 20, 1,80 }
    };

    const int SUBDIV = 20; // 分割数

	DirectX::XMFLOAT3 SoundPos = {0.0f,0.0f,0.0f};
};