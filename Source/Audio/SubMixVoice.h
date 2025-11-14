#pragma once
#include<wrl.h>
#include<xaudio2.h>
#include<xaudio2fx.h>
#include<xapofx.h>
#include<vector>
#include<memory>
#include<xapobase.h>

#include"SoundEffect.h"

class SubMixVoice
{
public:
	SubMixVoice(IXAudio2*xaudio2);
	~SubMixVoice();

	void setVolume(float volume);
	IXAudio2SubmixVoice* getSubMiXVoice() { return pSubMixVoice; }

	//値の更新等
	void update();

	//エフェクトの積み込み
	void addEffect(std::unique_ptr<SoundEffect>&& effect);

	//エフェクトの適用
	void applyEffect();

	SoundEffect* getEffect(SoundEffectType type);
protected:
	void equalizer();
private:
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> descriptorArray;
	std::vector<std::unique_ptr<SoundEffect>> effects;
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