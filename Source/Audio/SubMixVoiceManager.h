#pragma once
#include"SubMixVoice.h"
#include<memory>

class SubMixVoiceManager
{
public:
	SubMixVoiceManager();
	~SubMixVoiceManager();

	void update();
	void registerSubMixVoice(std::shared_ptr<SubMixVoice> subMixVoice);
	SubMixVoice* getSubMixVoice(int num) { return subMixVoiceArray.at(num).get(); }

	void Gui();
private:
	std::vector<std::shared_ptr<SubMixVoice>>subMixVoiceArray;
};