#pragma once
#include"Audio/Audio.h"
#include<unordered_map>

class AudioResourceList
{
public:
	AudioResourceList();
	~AudioResourceList();

	void loadResource(char* path);
	Audio* getAudio(char* name);
private:
	std::unordered_map<char*,Audio*>audioList;
};