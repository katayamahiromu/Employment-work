#pragma once

#include <memory>
#define  XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>
#include "AudioResource.h"
#include"SubMixVoice.h"

enum Fliter_Type
{
	LOW_PASS_FILTER = 0,
	HIGH_PASS_FILTER,
	BAND_PASS_FILTER,
	NOTCH_FILTER,
	LOW_PASS_ONE_POLE_FILTER,
	HIGH_PASS_ONE_POLE_FILTER,
};

// �I�[�f�B�I
class Audio
{
public:
	Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~Audio();

	std::string getName() { return resource->getName(); }

	//�t�Đ��p�Ƀf�[�^�̕ό`
	void createReversWav() { resource->reversData(); }

	/// <summary>
	/// �I�[�f�B�I�̍Đ�
	/// </summary>
	/// <param name="loop">true...���[�v</param>
	void play(bool loop);

	/// <summary>
	/// �I�[�f�B�I�̒�~
	/// </summary>
	void stop();

	/// <summary>
	/// �����V���b�g�̃I�[�f�B�I�Đ� 
	/// </summary>
	void DCPlay();

	/// <summary>
	/// �r������Đ�
	/// </summary>
	void middlePlay(float time);

	/// <summary>
	/// �w�肵���������[�v
	/// </summary>
	void sustainLoop(float loop_start, float loop_end);

	/// <summary>
	/// �t�Đ��@
	/// �ʏ�Đ��Ɠ����f�[�^��������Ȃ���Ή΂����Ȃ�̂Œ���
	/// </summary>
	void reversPlay(Audio*audio);

	/// <summary>
	/// �{�����[���̕ύX
	/// </summary>
	void setVolume(float volum);

	/// <summary>
	/// �s�b�`�̕ύX
	/// </summary>
	void setPitch(float pitch);

	/// <summary>
	/// �p�j���O
	/// </summary>
	void setPan(float pan);

	/// <summary>
	/// �s�b�`�̃��Z�b�g
	/// </summary>
	void resetPitch();

	/// <summary>
	/// �ꎞ��~
	/// </summary>
	void pause();

	/// <summary>
	/// ���X�^�[�g
	/// </summary>
	void reStart();

	/// <summary>
	/// �t�B���^�[�̐ݒ�
	/// </summary>
	void setFilter(int type);

	//�Đ�����
	bool isPlay();

	/// <summary>
	/// ���݂̍Đ��ʒu���擾�i���[�v�ɑΉ����ĂȂ��̂Œ��Ӂj
	/// </summary>
	UINT32 getPlaySamplingPosition();

	/// <summary>
	/// ���݂܂ł̍Đ����Ԃ��擾
	/// </summary>
	float getPlayTime();


	/// <summary>
	/// �o�͐���T�u�~�b�N�X�{�C�X�ɐݒ肷��
	/// </summary>
	void setSubmixVoice(SubMixVoice*sv);

	AudioResource* getResource() { return resource.get(); }
protected:
	IXAudio2SourceVoice* sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
	float volume = 1.0f;
};