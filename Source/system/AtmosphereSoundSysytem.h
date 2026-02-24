#pragma once
#include<memory>
#include"ProceduralAudio.h"

class AtmosphereSound
{
public:
	AtmosphereSound();
	~AtmosphereSound();

	void start();
	void stop();
	void update();
	void gui();
private:
	std::vector<uint8_t> noiseCreate();
private:
	std::unique_ptr<ProceduralAudio>noise;

	int playIndex = 0;	 //¶¬”Ô†
	int genIndex = 1;	 //Ä¶”Ô†

	float generateInterval = 0.1f;
};