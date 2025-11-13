#pragma once

#include<xaudio2.h>
#include<xaudio2fx.h>
#include<xapofx.h>

class SoundEffect
{
public:
	SoundEffect() {};
	virtual ~SoundEffect();

	IUnknown* getIUnknown()const { return pXAPO; }
protected:
	void release();
protected:
	IUnknown* pXAPO = nullptr;
};

class Reverb : public SoundEffect
{
public:
	Reverb();
	~Reverb();
};

class Echo : public SoundEffect
{
public:
	Echo();
	~Echo();
};