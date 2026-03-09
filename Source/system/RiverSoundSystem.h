#pragma once
#include"Audio/Audio3DSystem.h"
#include"ProceduralAudio.h"

class RiverSoundSystem
{
public:
	RiverSoundSystem();
	~RiverSoundSystem();

	void update();

	void debugRender();

	void gui();

    void start();
private:
    std::vector<uint8_t> createWave();
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

	DirectX::XMFLOAT3 SoundPos = {0.0f,0.0f,0.0f};

    std::unique_ptr<ProceduralAudio>noise;
    SoundEmitter emitter;
};