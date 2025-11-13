#pragma once
#include"SubMixVoice.h"
#include<memory>

class SubMixVoiceManager
{
public:
	SubMixVoiceManager();
	~SubMixVoiceManager();

	void createSubMixVoice();

	SubMixVoice* getSubMixVoice(int num) { return subMixVoiceArray.at(num).get(); }
private:
	std::vector<std::unique_ptr<SubMixVoice>>subMixVoiceArray;
};