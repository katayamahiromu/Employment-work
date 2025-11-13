#include"SubMixVoice.h"

SubMixVoice::SubMixVoice(IXAudio2* xaudio)
{
	//空のサブミックスボイスを作成
	UINT32 channels = 2;
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

void SubMixVoice::addEffect(SoundEffect* Effect)
{
	//ディスクリプタの設定
	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = Effect->getIUnknown();

	descriptorArray.push_back(descriptor);
}

void SubMixVoice::applyEffect()
{
	//配列分呼び出されるたび再設定
	XAUDIO2_EFFECT_CHAIN chain{};
	chain.EffectCount = static_cast<UINT32>(descriptorArray.size());
	chain.pEffectDescriptors = descriptorArray.data();
	pSubMixVoice->SetEffectChain(&chain);
}

void SubMixVoice::equalizer()
{
	IUnknown* pXAPO;
	CreateFX(__uuidof(FXEQ), &pXAPO);

	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = pXAPO;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;

	//簡易的なモデル
	FXEQ_PARAMETERS eqParams = {};
	eqParams.FrequencyCenter0 = 100.0f;
	eqParams.FrequencyCenter1 = 800.0f;
	eqParams.FrequencyCenter2 = 2000.0f;
	eqParams.FrequencyCenter3 = 8000.0f;

	eqParams.Bandwidth0 = eqParams.Bandwidth1 = eqParams.Bandwidth2 = eqParams.Bandwidth3 = 1.0f;

	eqParams.Gain0 = 6.0f;
	eqParams.Gain1 = 0.0f;
	eqParams.Gain2 = -3.0f;
	eqParams.Gain3 = 0.0f;
	pSubMixVoice->SetEffectChain(&chain);


	pSubMixVoice->SetEffectParameters(0, &eqParams, sizeof(eqParams));
	pXAPO->Release();
}