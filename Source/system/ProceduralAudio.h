#pragma once
#include"Audio/SignalProcessor.h"
#include"Audio/ModalMode.h"
#include<xaudio2.h>
#include<memory>

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
	/// 再生中はTrueを返す
	/// </summary>
	bool isPlay()
	{
		XAUDIO2_VOICE_STATE state;
		sourceVoice->GetState(&state);
		return (state.BuffersQueued > 0);
	}
};

class ProceduralAudio
{
public:
	using function = std::function<std::vector<UINT8>()>;

	ProceduralAudio(int maxSourceCount);
	~ProceduralAudio();

	void play(int index);
	void erase(int index);
	bool isPlay(int index);

	void createModalWave(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain);

	void loadModalData(const char* filename, float durationSeconds, float masterGain);

	SignalProcesser* getSignalProcesser(){ return signal.get(); }

	//指定したインデックスに波形データを格納
	void registerWaveData(function f,int index);
private:
	std::unique_ptr<SignalProcesser> signal;
	std::vector<AudioData>audioData;
	int maxSourceCount;
};