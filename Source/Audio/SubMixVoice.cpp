#include"SubMixVoice.h"
#include"../imgui/imgui.h"
#include"Audio/AudioManager.h"

SubMixVoice::SubMixVoice(IXAudio2* xaudio)
{
	//接続されているchの数を取得
	XAUDIO2_VOICE_DETAILS details;
	AudioManager::instance()->getMasteringVoice()->GetVoiceDetails(&details);

	//空のサブミックスボイスを作成
	UINT32 channels = details.InputChannels;
	UINT32 samplingRate = 44100;
	
	HRESULT hr;
	hr = xaudio->CreateSubmixVoice(&pSubMixVoice, channels, samplingRate);
}

SubMixVoice::~SubMixVoice()
{
	if (pSubMixVoice)
	{
		pSubMixVoice->DestroyVoice();
		pSubMixVoice = nullptr;
	}
}

void SubMixVoice::setVolume(float volume)
{
	pSubMixVoice->SetVolume(volume);
}

void SubMixVoice::update()
{
	for (auto& effect : effects)effect->update(pSubMixVoice);
}

void SubMixVoice::Gui()
{
	if (ImGui::TreeNode("SubMixVoice"))
	{
		for (auto& effect : effects)effect->Gui();
		ImGui::TreePop();
	}
	ImGui::Separator();
}

void SubMixVoice::addEffect(std::unique_ptr<SoundEffect>&& effect)
{

	XAUDIO2_VOICE_DETAILS details;
	//AudioManager::instance()->getMasteringVoice()->GetVoiceDetails(&details);
	pSubMixVoice->GetVoiceDetails(&details);
	//ディスクリプタの設定
	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = details.InputChannels;
	descriptor.pEffect = effect->getIUnknown();

	descriptorArray.push_back(descriptor);

	effects.push_back(std::move(effect));
}

void SubMixVoice::applyEffect()
{
	//配列分呼び出されるたび再設定
	XAUDIO2_EFFECT_CHAIN chain{};
	chain.EffectCount = static_cast<UINT32>(descriptorArray.size());
	chain.pEffectDescriptors = descriptorArray.data();
	pSubMixVoice->SetEffectChain(&chain);

	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
	sendDesc.Flags = 0;
	sendDesc.pOutputVoice = pSubMixVoice;

	XAUDIO2_VOICE_SENDS sendList = {};
	sendList.SendCount = 1;
	sendList.pSends = &sendDesc;

	pSubMixVoice->SetOutputVoices(&sendList);
}

SoundEffect* SubMixVoice::getEffect(SoundEffectType type)
{
	for (auto& effect : effects)
	{
		if (effect->getType() == type) return effect.get();
	}

	return nullptr;
}