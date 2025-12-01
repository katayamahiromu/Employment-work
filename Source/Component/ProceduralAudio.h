#pragma once
#include"Component.h"
#include"Audio/SignalProcessor.h"
#include"Audio/ModalMode.h"
#include<xaudio2.h>

struct AudioData
{
	IXAudio2SourceVoice* sourceVoice;

	void create(IXAudio2* audio,SignalProcesser& sp)
	{
		audio->CreateSourceVoice(&sourceVoice, &sp.getWaveFormat());
	}

	void destroy()
	{
		if (sourceVoice != nullptr)
		{
			sourceVoice->DestroyVoice();
			sourceVoice = nullptr;
		}
	}

	void play(SignalProcesser& sp)
	{
		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = sp.getAudioBytes();
		buffer.pAudioData = sp.getAudioData();
		sourceVoice->SubmitSourceBuffer(&buffer);
		sourceVoice->Start();
	}

	/// <summary>
	/// çƒê∂íÜÇÕTrueÇï‘Ç∑
	/// </summary>
	bool isPlay()
	{
		XAUDIO2_VOICE_STATE state;
		sourceVoice->GetState(&state);
		return (state.BuffersQueued > 0);
	}
};

class ProceduralAudio : public Component
{
public:
	ProceduralAudio(int maxSourceCount);
	~ProceduralAudio();

	const char* getName() const override { return "Procedural Audio"; }

	void update(float elapsedTime)override;

	void OnGUI()override;

	void play(int index);

	void createModalWave(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain);
private:
	std::unique_ptr<SignalProcesser> signal;
	std::vector<AudioData>audioData;
	int maxSourceCount;
};