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

//簡易リバーブ
class Reverb : public SoundEffect
{
public:
	Reverb(int slot);
	~Reverb();

	void update(IXAudio2SubmixVoice* sv)override;

	SoundEffectType getType()const override { return SoundEffectType::REVERB; }

	/*void setWetLevel(float value) { setParameter(wetLevel, value); }
	void setDecayTime(float value) { setParameter(decayTime, value); }*/

	void Gui()override;
private:
	////残響と実音の混ざり具合
	//float wetLevel = 50.0f; //0~100;

	////残響時間
	//float decayTime = 1.5f; 

	//壁の硬さ
	float diffusion = 0.9f;

	//部屋の大きさ
	float roomSize = 0.6f;
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
	float wetDryMix = 0.5f; // 0.0f ～ 1.0f

	//残響を残すか
	float feedback = 0.8f;//0.0f ~ 1.0f

	//何秒遅らせるか
	float delay = 1000.0f; // 1ms ～ 2000ms
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