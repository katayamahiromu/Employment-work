#include"SoundEffect.h"
#include<xaudio2fx.h>

SoundEffect::~SoundEffect()
{
	release();
}

void SoundEffect::release()
{
	if (pXAPO) 
	{
		pXAPO->Release();
		pXAPO = nullptr;
	}
}

Reverb::Reverb(int slot):SoundEffect(slot)
{
	XAudio2CreateReverb(&pXAPO);
}

Reverb::~Reverb()
{
}

void Reverb::update(IXAudio2SubmixVoice* sv)
{
	//値が変更されてないなら即リターン
	if (!isAlter)return;
	
	XAUDIO2FX_REVERB_PARAMETERS param;

	param.WetDryMix = wetLevel;
	param.RoomSize = roomSize;
	param.DecayTime = decayTime;
	
	HRESULT hr = sv->SetEffectParameters(slot,&param,sizeof(param));
	isAlter = false;
}


Echo::Echo(int slot) :SoundEffect(slot)
{
	CreateFX(__uuidof(FXEcho), &pXAPO);
}

Echo::~Echo()
{
}

void Echo::update(IXAudio2SubmixVoice* sv)
{
	//値が変更されてないなら即リターン
	if (!isAlter || !sv)return;

	FXECHO_PARAMETERS param;

	param.Delay = delay;
	param.Feedback = feedback;
	param.WetDryMix = wetDryMix;

	HRESULT hr = sv->SetEffectParameters(slot, &param, sizeof(param));
	isAlter = false;
}