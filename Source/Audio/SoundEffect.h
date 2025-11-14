#pragma once

#include<xaudio2.h>
#define  XAUDIO2_HELPER_FUNCTIONS
#include<xapofx.h>

enum SoundEffectType
{
	REVERB,
	ECHO,
};

class SoundEffect
{
public:
	SoundEffect(int slot):slot(slot) {};
	virtual ~SoundEffect();

	virtual void update(IXAudio2SubmixVoice*sv) = 0;

	IUnknown* getIUnknown()const { return pXAPO; }

	virtual SoundEffectType getType() const = 0;

private:
	void release();
protected:
	void notifyAlter() { isAlter = true; }

	//ílÇÃïœçX
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

class Reverb : public SoundEffect
{
public:
	Reverb(int slot);
	~Reverb();

	void update(IXAudio2SubmixVoice* sv)override;

	SoundEffectType getType()const override { return SoundEffectType::REVERB; }

	void setWetLevel(float value) { setParameter(wetLevel, value); }
	void setRoomSize(float value) { setParameter(roomSize, value); }
	void setDecayTime(float value) { setParameter(decayTime, value); }
private:
	float wetLevel = 0.3f;
	float roomSize = 1.0f;
	float decayTime = 100.0f;
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
private:
	float wetDryMix = 0.0f; // 0.0f Å` 100.0f
	float feedback = 0.0f;//0.0f ~ 1.0f
	float delay = 1.0f; // 1ms Å` 2000ms
};