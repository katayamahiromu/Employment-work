#pragma once

#include <xaudio2.h>
#include "Audio.h"
#include "Audio3DSystem.h"
#include "SubMixVoice.h"
#include <unordered_map>

// �I�[�f�B�I�Ǘ�
class AudioManager
{
private:
	AudioManager() {}
	~AudioManager();

public:
	static AudioManager* instance()
	{
		static AudioManager inst;
		return &inst;
	}

	AudioManager* initialize();

	/// <summary>
	/// �ǂݍ���
	/// </summary>
	/// <param name="filename">�ǂݍ��ރt�@�C����</param>
	/// <returns>�쐬�����I�[�f�B�I</returns>
	std::unique_ptr<Audio> loadAudioSource(const char* filename);

	/// <summary>
	/// 3D�����p�̓ǂݍ���
	/// </summary>
	/// <param name="filename">�ǂݍ��ރt�@�C����</param>
	/// <param name="emitter">�G�~�^�[�̏��</param>
	/// <returns></returns>
	std::unique_ptr<Audio3D>loadAudioSource3D(const char* filename, SoundEmitter* emitter);

	/// <summary>
	/// �I�[�f�B�I��Z�߂��T�u�~�b�N�X�{�C�X�̐���
	/// �G�t�F�N�g������������o����
	/// </summary>
	std::unique_ptr<SubMixVoice>createSubMixVoice();

	IXAudio2MasteringVoice* getMasteringVoice() { return masteringVoice; }

	void registerAudio(SoundListner* source, int slot) { listenerArray.insert({ slot,source }); }
	SoundListner* findAudio(int slot) { return listenerArray.at(slot); }

private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;

	std::unordered_map<int, SoundListner*>listenerArray;
};
