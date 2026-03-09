#pragma once
#include<DirectXMath.h>
#include<unordered_map>
#include<vector>

enum class Lisner : int
{
	PLAYER,
	CAMERA
};

//音源
struct SoundEmitter
{
	DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 velocity = { 0.0f,0.0f,0.0f };
	float minDistance;
	float maxDistance;
};

//リスナー
struct SoundListner
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 frontVec;
	DirectX::XMFLOAT3 rightVec;
	float innerRadius;
	float outerRadius;
	float filterParam;
};

struct SoundDSPSetting
{
	int srcChannelCount;
	int dstChannelCount;
	float* outputMatrix;
};

struct DSPResult
{
    //距離
	float distance = 0.0f;

    //ドップラー率
	float dopplerScale = 1.0f;

    //指向性
	float radian = 0.0f;

    // 減衰率
	float scale = 1.0f;

    //LPの周波数
	float filterParam = 0.0f;
};

struct SpeakerDir {
	float azimuth;   // スピーカーの方向（ラジアン）
	float elevation; // 必要なら上下方向（今回は 0）
};

// レイアウトごとのスピーカー方向テーブル
static std::unordered_map<int, std::vector<SpeakerDir>> SpeakerLayout = 
{
     { 1, {
        { 0.0f, 0.0f }
    }},

    { 2, {
        { DirectX::XM_PI * 7.0f / 4.0f, 0.0f }, // L  (315°)
        { DirectX::XM_PI * 1.0f / 4.0f, 0.0f }  // R  (45°)
    }},

    { 4, {
        { DirectX::XM_PI * 3.0f / 2.0f, 0.0f }, // SL (270°)
        { DirectX::XM_PI * 7.0f / 4.0f, 0.0f }, // FL (315°)
        { DirectX::XM_PI * 1.0f / 4.0f, 0.0f }, // FR (45°)
        { DirectX::XM_PI * 1.0f / 2.0f, 0.0f }  // SR (90°)
    }},

    { 6, {
        { DirectX::XM_PI * 3.0f / 2.0f, 0.0f }, // SL (270°)
        { DirectX::XM_PI * 7.0f / 4.0f, 0.0f }, // FL (315°)
        { 0.0f,                         0.0f }, // FC (0°)
        { 0.0f,                         0.0f }, // LFE（使わない）
        { DirectX::XM_PI * 1.0f / 4.0f, 0.0f }, // FR (45°)
        { DirectX::XM_PI * 1.0f / 2.0f, 0.0f }  // SR (90°)
    }},

    { 8, {
        { DirectX::XM_PI * 5.0f / 4.0f, 0.0f }, // BL (225°)
        { DirectX::XM_PI * 3.0f / 2.0f, 0.0f }, // SL (270°)
        { DirectX::XM_PI * 7.0f / 4.0f, 0.0f }, // FL (315°)
        { 0.0f,                         0.0f }, // FC (0°)
        { 0.0f,                         0.0f }, // LFE
        { DirectX::XM_PI * 1.0f / 4.0f, 0.0f }, // FR (45°)
        { DirectX::XM_PI * 1.0f / 2.0f, 0.0f }, // SR (90°)
        { DirectX::XM_PI * 3.0f / 4.0f, 0.0f }  // BR (135°)
    }}
};

constexpr float SOUND_SPEED = 340.0f;