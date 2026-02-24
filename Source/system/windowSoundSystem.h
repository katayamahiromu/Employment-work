#pragma once
#include<memory>
#include"ProceduralAudio.h"
#include"../Camera.h"

class WindowSoundSystem
{
public:
	WindowSoundSystem();
	~WindowSoundSystem();

	void start();
	void stop();
	void update();
	void gui();

	void calcPan(Camera& camera);
private:
	std::vector<uint8_t> create();
private:
	float pan = 0.0f;
	float frontBack = 0.0f;
	std::unique_ptr<ProceduralAudio>windowSound;

	float windowSpeed = 0.5f;
	float gustAmount = 0.6f;
	float brightness = 1.0f;

	float St = 0.2f;     // ストローハル数
	float D = 0.005f;      // 物体サイズ（隙間）
	float U0 = 25.0f;     // 基本風速
	float rQ = 0.985f;    // 共鳴の鋭さ
	float windRange = 30.0f; // 風速揺らぎ幅

	float generateInterval = 0.5f;
	float smoothedSpeed = 0.0f;
	float speedSmoothFactor = 0.05f;
};