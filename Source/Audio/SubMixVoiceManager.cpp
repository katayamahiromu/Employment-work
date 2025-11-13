#include"SubMixVoiceManager.h"
#include"AudioManager.h"
#include"SoundEffect.h"

SubMixVoiceManager::SubMixVoiceManager()
{

}

SubMixVoiceManager::~SubMixVoiceManager()
{

}

void SubMixVoiceManager::createSubMixVoice()
{
	std::unique_ptr<SubMixVoice> sm = AudioManager::instance()->createSubMixVoice();
	sm->addEffect(std::make_unique<Reverb>());
	sm->addEffect(std::make_unique<Echo>());
	sm->applyEffect();

	subMixVoiceArray.emplace_back(std::move(sm));
}