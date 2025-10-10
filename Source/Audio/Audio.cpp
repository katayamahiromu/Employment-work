#include "../misc.h"
#include "Audio.h"
#include"AudioManager.h"
#include<x3daudio.h>

// �R���X�g���N�^
Audio::Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource)
	: resource(resource)
{
	HRESULT hr;

	// �\�[�X�{�C�X�𐶐�
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->getWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

// �f�X�g���N�^
Audio::~Audio()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

// �Đ�
void Audio::play(bool loop)
{
	stop();

	// �\�[�X�{�C�X�Ƀf�[�^�𑗐M
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	
	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	sourceVoice->SetVolume(1.0f);
}

// ��~
void Audio::stop()
{
	sourceVoice->FlushSourceBuffers();
	sourceVoice->Stop();
}

//�����V���b�g�̃I�[�f�B�I�Đ� 
void Audio::DCPlay()
{
	stop();
	sourceVoice->FlushSourceBuffers();
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//�r������Đ�
void Audio::middlePlay(float time)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.PlayBegin = static_cast<UINT32>(resource->getWaveFormat().nSamplesPerSec * time);
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	sourceVoice->SubmitSourceBuffer(&buffer);
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//�w�肵���������[�v
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

//�t�Đ�
void Audio::reversPlay(Audio*audio)
{
	if (!audio)return;

	//������d�ɕ�������׌��ɂȂ鉹�����~�߂�
	audio->stop();

	//�ő�Đ����� - ���̍Đ�����
	middlePlay(resource->getMaxPlayTime() - audio->getPlayTime());
}

//�{�����[���̕ύX
void Audio::setVolume(float volume)
{
	//setVolume���d�����ߏ����𕪊򂳂���
	if (this->volume != volume)
	{
		this->volume = volume;
		sourceVoice->SetVolume(volume);
	}
}

//�s�b�`
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


//�s�b�`�̃��Z�b�g
void Audio::resetPitch()
{
	sourceVoice->SetFrequencyRatio(1.0f);
}

//�|�[�Y
void Audio::pause()
{
	sourceVoice->Stop(XAUDIO2_PLAY_TAILS);
}

//���X�^�[�g
void Audio::reStart()
{
	sourceVoice->Start();
}

//�t�B���^�[
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
	//BuffersQueued �� 0 �łȂ��A�܂��� SamplesPlayed �� 0 �łȂ��ꍇ�A�Đ����Ƃ݂Ȃ�
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
	return static_cast<float>(state.SamplesPlayed / resource->getWaveFormat().nSamplesPerSec);
}

void Audio::setSubmixVoice(SubMixVoice* sv)
{
	HRESULT hr;
	//�T�u�~�b�N�X�{�C�X�ɓo�^����
	XAUDIO2_SEND_DESCRIPTOR send = { 0,sv->getSubMiXVoice()};
	XAUDIO2_VOICE_SENDS sendlist = { 1,&send };
	hr = sourceVoice->SetOutputVoices(&sendlist);
}