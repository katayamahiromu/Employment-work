#include"SoundEffect.h"

SoundEffect::~SoundEffect()
{
	release();
}

void SoundEffect::release()
{
	if (pXAPO) {
		pXAPO->Release();
		pXAPO = nullptr;
	}
}

Reverb::Reverb()
{
	XAudio2CreateReverb(&pXAPO);
}

Reverb::~Reverb()
{
}


Echo::Echo()
{
	CreateFX(__uuidof(FXEcho), &pXAPO);
}

Echo::~Echo()
{
}
