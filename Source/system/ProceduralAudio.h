#pragma once
#include"Audio/SignalProcessor.h"
#include"Audio/ModalMode.h"
#include"Audio/AudioCallback.h"
#include"Audio/AudioManager.h"
#include<memory>

struct AudioData
{
    IXAudio2SourceVoice* sourceVoice;
    bool playing = false;
    std::unique_ptr<ProceduralAudioCallback> callback;
    float pan = 0.0f;
    float frontBack = 0.0f;

    void create(IXAudio2* audio, SignalProcesser& sp)
    {
        callback = std::make_unique<ProceduralAudioCallback>();

        audio->CreateSourceVoice(
            &sourceVoice,
            &sp.getWaveFormat(),
            0,
            XAUDIO2_DEFAULT_FREQ_RATIO,
            callback.get()
        );
    }

    void play(SignalProcesser& sp)
    {
        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = sp.getAudioBytes();
        buffer.pAudioData = sp.getAudioData();
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.pContext = this;

        sourceVoice->SubmitSourceBuffer(&buffer);
        sourceVoice->Start();
        playing = true;
    }

    void stop()
    {
        sourceVoice->Stop();
    }

    bool isPlay()
    {
        return playing;
    }

    void destroy()
    {
        if (sourceVoice)
        {
            sourceVoice->DestroyVoice();
            sourceVoice = nullptr;
        }
    }

	void Pan(float pan,float frontBack)
	{
        pan = std::clamp(pan, -1.0f, 1.0f);
        frontBack = std::clamp(frontBack, 0.0f, 1.0f);

        if (this->pan == pan && this->frontBack == frontBack)return;
        this->pan = pan;
        this->frontBack = frontBack;

        float outputPan[8] = {};

        XAUDIO2_VOICE_DETAILS sourceDetails;
        sourceVoice->GetVoiceDetails(&sourceDetails);

        XAUDIO2_VOICE_DETAILS masterDetails;
        auto* mv = AudioManager::instance()->getMasteringVoice();
        mv->GetVoiceDetails(&masterDetails);

        float backGain = 1.0f - 0.3f * frontBack;

        float minGain = 0.4f;
        float left = minGain + (1.0f - minGain) * (0.5f - pan * 0.5f) * backGain;
        float right = minGain + (1.0f - minGain) * (0.5f + pan * 0.5f) * backGain;

        DWORD mask{};
        mv->GetChannelMask(&mask);

        switch (mask)
        {
        case SPEAKER_MONO:
            outputPan[0] = 1.0f;
            break;
        case SPEAKER_STEREO:
        case SPEAKER_2POINT1:
        case SPEAKER_SURROUND:
            outputPan[0] = left;   // FL
            outputPan[1] = right;  // FR
            break;
        case SPEAKER_QUAD:
            outputPan[0] = outputPan[4] = left;   // FL, BL
            outputPan[1] = outputPan[5] = right;  // FR, BR
            break;

        case SPEAKER_4POINT1:
            outputPan[0] = outputPan[4] = left;   // FL, BL
            outputPan[1] = outputPan[5] = right;  // FR, BR
            break;

        case SPEAKER_5POINT1:
        case SPEAKER_5POINT1_SURROUND:
            outputPan[0] = left;   // FL
            outputPan[1] = right;  // FR
            outputPan[4] = left;   // SL
            outputPan[5] = right;  // SR
            break;

        case SPEAKER_7POINT1:
        case SPEAKER_7POINT1_SURROUND:
            outputPan[0] = left;   // FL
            outputPan[1] = right;  // FR
            outputPan[4] = left;   // BL
            outputPan[5] = right;  // BR
            outputPan[6] = left;   // SL
            outputPan[7] = right;  // SR
            break;
        }

        sourceVoice->SetOutputMatrix(
            mv,
            sourceDetails.InputChannels,
            masterDetails.InputChannels,
            outputPan
        );

        float frontCutoff = 20000.0f;
        float backCutoff = 1500.0f;

        float cutoff = frontCutoff + (backCutoff - frontCutoff) * frontBack;

        XAUDIO2_FILTER_PARAMETERS filter{};
        filter.Type = LowPassFilter;
        filter.Frequency = cutoff / SamplingRate;
        filter.OneOverQ = 0.5f;

        sourceVoice->SetFilterParameters(&filter);
	}

};

class ProceduralAudio
{
public:
	using function = std::function<std::vector<UINT8>()>;

	ProceduralAudio(int maxSourceCount);
	~ProceduralAudio();

	void play(int index,bool tryS = true);
    void stop(int index);
	void erase(int index);
	bool isPlay(int index);
	void pan(float pan,float frontBack);
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