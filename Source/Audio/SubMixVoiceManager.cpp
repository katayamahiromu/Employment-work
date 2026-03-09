#include"SubMixVoiceManager.h"
#include"AudioManager.h"
#include"SoundEffect.h"

SubMixVoiceManager::SubMixVoiceManager()
{

}

SubMixVoiceManager::~SubMixVoiceManager()
{

}

void SubMixVoiceManager::update()
{
	for (auto& sv : subMixVoiceArray)sv->update();
}

void SubMixVoiceManager::Gui()
{
	for (auto& sv : subMixVoiceArray)sv->Gui();
}

void SubMixVoiceManager::registerSubMixVoice(std::shared_ptr<SubMixVoice> subMixVoice)
{
	subMixVoiceArray.emplace_back(subMixVoice);
}