#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include<xaudio2fx.h>
#include<xapofx.h>

#include<xapobase.h>
class SubMixVoice
{
public:
	SubMixVoice(IXAudio2*xaudio2);
	~SubMixVoice();

	void setVolume(float volume);
	void Reverb();
	void echo();
	void equalizer();
	IXAudio2SubmixVoice* getSubMiXVoice() { return pSubMixVoice; }
private:
	IXAudio2SubmixVoice* pSubMixVoice = nullptr;
};

class TestXAPO : public CXAPOBase
{
public:
	TestXAPO();
	~TestXAPO();

	
private:
	static XAPO_REGISTRATION_PROPERTIES xapoRegProp;//プロパティ
	WAVEFORMAT inputFmt;
	WAVEFORMAT outputFmt;
	
};