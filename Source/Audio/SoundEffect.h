#pragma once

#include<xaudio2.h>
#define  XAUDIO2_HELPER_FUNCTIONS
#include<xapofx.h>
#include <xaudio2fx.h>
#include <DirectXMath.h>

enum SoundEffectType
{
	REVERB,
	ECHO,
	EQUALIZER,
};

class SoundEffect
{
public:
	SoundEffect(int slot):slot(slot) {};
	virtual ~SoundEffect();

	virtual void update(IXAudio2SubmixVoice*sv) = 0;

	IUnknown* getIUnknown()const { return pXAPO; }

	virtual SoundEffectType getType() const = 0;

	virtual void Gui(){}

private:
	void release();
protected:
	void notifyAlter() { isAlter = true; }

	//値の変更
	template<typename T>
	void setParameter(T& target, const T& value)
	{
		if (target == value)return;
		target = value;
		notifyAlter();
	}
protected:
	IUnknown* pXAPO = nullptr;
	bool isAlter = false;
	int slot;
};

//リバーブ
class Reverb : public SoundEffect
{
public:
	Reverb(int slot);
	~Reverb();

	void update(IXAudio2SubmixVoice* sv)override;

	SoundEffectType getType()const override { return SoundEffectType::REVERB; }

	void setWetDryMix(float value) { setParameter(wetDryMix, value); }
	void setDecayTime(float value) { setParameter(decayTime, value); }
	void setReverbGain(float value) { setParameter(reverbGain, value); }
	void setRoomFilterHF(float value) { setParameter(roomFilterHF, value); }

	void Gui()override;
private:
	float wetDryMix = 25.0f;	//%
	float decayTime = 1.0f;		//seconds
	float reverbGain = -8.0f;	//db
	float roomFilterHF = -3.0f; //db
};

class Echo : public SoundEffect
{
public:
	Echo(int slot);
	~Echo();

	void update(IXAudio2SubmixVoice* sv)override;
	SoundEffectType getType()const override { return SoundEffectType::ECHO; }

	void setWetDryMix(float value) { setParameter(wetDryMix, value); }
	void setFeedback(float value) { setParameter(feedback, value); }
	void setDelay(float value) { setParameter(delay, value); }

	void Gui()override;
private:
	//エフェクトの強さ
	float wetDryMix = 0.5f;

	//残響を残すか
	float feedback = 0.8f;

	//何秒遅らせるか
	float delay = 1000.0f;
};

class Equalizer : public SoundEffect
{
public:
	Equalizer(int slot);
	~Equalizer();

	void update(IXAudio2SubmixVoice* sv)override;
	SoundEffectType getType()const override { return SoundEffectType::EQUALIZER; }

	void Gui()override;
private:
	DirectX::XMFLOAT4 frequencyCenter = { 120.0f,800.0f,2500.0f,10000.0f };
	DirectX::XMFLOAT4 bandwidth = { 1.0f,1.0f,1.0f,1.0f };
	DirectX::XMFLOAT4 gain = { 4.0f,0.0f,2.0f,3.0f };
};