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

void SubMixVoiceManager::createSubMixVoice()
{
	std::unique_ptr<SubMixVoice> sm = AudioManager::instance()->createSubMixVoice();
	sm->addEffect(std::make_unique<Reverb>(0));
	sm->addEffect(std::make_unique<Echo>(1));

	sm->applyEffect();

	subMixVoiceArray.emplace_back(std::move(sm));
}