#pragma once
#include"Audio/Audio.h"
#include<string>
#include<map>

class AudioResourceList
{
public:
	AudioResourceList();
	~AudioResourceList();

	static AudioResourceList* instance()
	{
		static AudioResourceList inst;
		return &inst;
	}

	void loadResource(const char* path,bool revers = false);
	Audio* getAudio(std::string name);
	void allClear();

	void onGui();
private:
	struct AudioInfo
	{
		std::unique_ptr<Audio>source;
		float volum;
		float pan;
	};
private:
	std::map <std::string, AudioInfo> audioList;
};