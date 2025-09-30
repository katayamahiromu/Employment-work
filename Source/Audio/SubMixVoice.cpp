#include"SubMixVoice.h"

SubMixVoice::SubMixVoice(IXAudio2* xaudio)
{
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

void SubMixVoice::Reverb()
{
	IUnknown* pXAPO;
	XAudio2CreateReverb(&pXAPO);

	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = pXAPO;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;

	pSubMixVoice->SetEffectChain(&chain);
	pXAPO->Release();
}

void SubMixVoice::echo()
{
	IUnknown* pXAPO;
	CreateFX(__uuidof(FXEcho), &pXAPO);

	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = pXAPO;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;

	pSubMixVoice->SetEffectChain(&chain);
	pXAPO->Release();
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

	//ŠÈˆÕ“I‚Èƒ‚ƒfƒ‹
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