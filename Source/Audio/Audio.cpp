#include "../misc.h"
#include "Audio.h"
#include"AudioManager.h"
#include<x3daudio.h>

// コンストラクタ
Audio::Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource,bool effect)
	: resource(resource)
{
	HRESULT hr;

	callback = std::make_unique<AudioCallback>();

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(
		&sourceVoice,
		&resource->getWaveFormat(),
		0,
		XAUDIO2_DEFAULT_FREQ_RATIO,
		callback.get());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

}

Audio::Audio(IXAudio2* xaudio, WAVEFORMATEX wfx)
{
	HRESULT hr;

	callback = std::make_unique<AudioCallback>();

	//ソールボイスを生成
	hr = xaudio->CreateSourceVoice(
		&sourceVoice,
		&wfx,
		0,
		XAUDIO2_DEFAULT_FREQ_RATIO,
		callback.get());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

// デストラクタ
Audio::~Audio()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

void Audio::update(float elapsedTime)
{
	if (isFadeIn)
	{
		//フェードイン処理
		volume += (1.0f / fadeTime) * elapsedTime;

		//ボリュームが範囲内にあるように調整
		if (volume > 1.0f)
		{
			volume = 1.0f;
			isFadeIn = false;
		}
		setVolume(volume);
	}
	else if (isFadeOut)
	{
		//フェードアウト処理
		volume -= (1.0f / fadeTime) * elapsedTime;

		//ボリュームが範囲内になるように調整
		if (volume < 0.0f)
		{
			volume = 0.0f;
			isFadeOut = false;

			//停止時に破棄準備
			stop();
			destruction();
		}
		setVolume(volume);
	}
}

// 再生
void Audio::play(bool loop)
{
	stop();

	// ソースボイスにデータを送信
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pContext = this;

	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	sourceVoice->SetVolume(1.0f);
}

// 停止
void Audio::stop()
{
	sourceVoice->FlushSourceBuffers();
	sourceVoice->Stop();
}

//ワンショットのオーディオ再生 
void Audio::DCPlay()
{
	stop();
	sourceVoice->FlushSourceBuffers();
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pContext = this;

	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//途中から再生
void Audio::middlePlay(float time)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.PlayBegin = static_cast<UINT32>(resource->getWaveFormat().nSamplesPerSec * time);
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pContext = this;

	sourceVoice->SubmitSourceBuffer(&buffer);
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//指定した所をループ
void Audio::sustainLoop(float loop_start, float loop_end)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	buffer.LoopBegin = static_cast<UINT32>(resource->getWaveFormat().nSamplesPerSec * loop_start);
	buffer.LoopLength = static_cast<UINT32>(resource->getWaveFormat().nSamplesPerSec * loop_end);
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	sourceVoice->SubmitSourceBuffer(&buffer);
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//逆再生
void Audio::reversPlay(Audio*audio)
{
	if (!audio)return;

	//音が二重に聞こえる為元になる音源を止める
	audio->stop();

	//最大再生時間 - 元の再生時間
	middlePlay(resource->getMaxPlayTime() - audio->getPlayTime());
}

//ボリュームの変更
void Audio::setVolume(float volume)
{
	//setVolumeが重いため処理を分岐させる
	if (this->volume != volume)
	{
		this->volume = volume;
		sourceVoice->SetVolume(volume);
	}
}

//ピッチ
void Audio::setPitch(float Pitch)
{
	sourceVoice->SetFrequencyRatio(Pitch);
}

void Audio::setPan(float pan)
{
	float outputPan[8] = {};

	XAUDIO2_VOICE_DETAILS sourceDetails;
	sourceVoice->GetVoiceDetails(&sourceDetails);

	XAUDIO2_VOICE_DETAILS masterDetails;
	AudioManager::instance()->getMasteringVoice()->GetVoiceDetails(&masterDetails);

	float left = 0.5f - pan * 0.5f;
	float right = 0.5f + pan * 0.5f;

	DWORD ChannelMask{};
	AudioManager::instance()->getMasteringVoice()->GetChannelMask(&ChannelMask);
	switch (ChannelMask)
	{
		case SPEAKER_MONO:
			outputPan[0] = 1.0f;
			break;
		case SPEAKER_STEREO:
		case SPEAKER_2POINT1:
		case SPEAKER_SURROUND:
			outputPan[0] = left;
			outputPan[3] = right;
			break;
		case SPEAKER_QUAD:
			outputPan[0] = outputPan[2] = left;
			outputPan[1] = outputPan[3] = right;
			break;
		case SPEAKER_4POINT1:
			outputPan[0] = outputPan[3] = left;
			outputPan[1] = outputPan[4] = right;
			break;
		case SPEAKER_5POINT1:
		case SPEAKER_7POINT1:
		case SPEAKER_5POINT1_SURROUND:
			outputPan[0] = outputPan[4] = left;
			outputPan[1] = outputPan[5] = right;
			break;
		case SPEAKER_7POINT1_SURROUND:
			outputPan[0] = outputPan[4] = outputPan[6] = left;
			outputPan[1] = outputPan[5] = outputPan[7] = right;
			break;
	}

	sourceVoice->SetOutputMatrix(AudioManager::instance()->getMasteringVoice(), sourceDetails.InputChannels, masterDetails.InputChannels, outputPan);
}

void Audio::setPan(float pan, float frontBack)
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

//ピッチのリセット
void Audio::resetPitch()
{
	sourceVoice->SetFrequencyRatio(1.0f);
}

//ポーズ
void Audio::pause()
{
	sourceVoice->Stop(XAUDIO2_PLAY_TAILS);
}

//リスタート
void Audio::reStart()
{
	sourceVoice->Start();
}

//フィルター
void Audio::setFilter(int type)
{
	XAUDIO2_FILTER_PARAMETERS paramers{};

	switch (type)
	{
	case Fliter_Type::LOW_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::LowPassFilter;
		paramers.Frequency = 0.5f / resource->getWaveFormat().nSamplesPerSec * 6.0f;
		paramers.OneOverQ = 1.4142f;
		break;
	case Fliter_Type::HIGH_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::HighPassFilter;
		paramers.Frequency = 0.5f / resource->getWaveFormat().nSamplesPerSec * 6.0f;
		paramers.OneOverQ = 1.4142f;
		break;
	case Fliter_Type::BAND_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::BandPassFilter;
		paramers.Frequency = 0.5f;
		paramers.OneOverQ = 1.0f;
		break;
	case Fliter_Type::NOTCH_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::NotchFilter;
		paramers.Frequency = 0.5f;
		paramers.OneOverQ = 1.0f;
		break;
	case Fliter_Type::LOW_PASS_ONE_POLE_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter;
		paramers.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(0.5f, resource->getWaveFormat().nSamplesPerSec);
		break;
	case Fliter_Type::HIGH_PASS_ONE_POLE_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter;
		paramers.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(0.5f, resource->getWaveFormat().nSamplesPerSec);
		break;
	}


	sourceVoice->SetFilterParameters(&paramers);
}

bool Audio::isPlay()
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);
	//BuffersQueued が 0 でない、または SamplesPlayed が 0 でない場合、再生中とみなす
	if (state.BuffersQueued > 0 || state.SamplesPlayed > 0)return true;
	return false;
}

UINT32 Audio::getPlaySamplingPosition()
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);
	return static_cast<UINT32>(state.SamplesPlayed);
}


float Audio::getPlayTime()
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);
	float totalTime = static_cast<float>(state.SamplesPlayed) / static_cast<float>(resource->getWaveFormat().nSamplesPerSec);
	return fmodf(totalTime, resource->getDuration());
}

void Audio::setSubmixVoice(SubMixVoice* sv)
{

	if (sv == nullptr)return;
	HRESULT hr;
	//サブミックスボイスを登録する
	XAUDIO2_SEND_DESCRIPTOR send = { 0,sv->getSubMiXVoice()};
	XAUDIO2_VOICE_SENDS sendlist = { 1,&send };
	hr = sourceVoice->SetOutputVoices(&sendlist);
}

size_t Audio::getSamplePlay()
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);
	return state.SamplesPlayed;
}

//フェード
void Audio::setFade(Fade type, float fadeTime)
{
	//フェードが０秒以下なら何もしない
	if (fadeTime < 0.0f) return;

	switch (type)
	{
	case Audio::Fade::None:
		setVolume(1.0f);
		return;
	case Audio::Fade::In:
		setVolume(0.0f);
		isFadeIn = true;
		break;
	case Audio::Fade::Out:
		isFadeOut = true;
		break;
	default:
		break;
	}

	this->fadeTime = fadeTime;
}