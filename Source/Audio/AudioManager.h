#pragma once

#include <xaudio2.h>
#include "Audio.h"
#include "Audio3DSystem.h"
#include "SubMixVoice.h"
#include <unordered_map>

// オーディオ管理
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
	/// 読み込み
	/// </summary>
	/// <param name="filename">読み込むファイル名</param>
	/// <returns>作成したオーディオ</returns>
	std::unique_ptr<Audio> loadAudioSource(const char* filename);

	/// <summary>
	/// 3D音響用の読み込み
	/// </summary>
	/// <param name="filename">読み込むファイル名</param>
	/// <param name="emitter">エミターの情報</param>
	/// <returns></returns>
	std::unique_ptr<Audio3D>loadAudioSource3D(const char* filename, SoundEmitter* emitter);

	/// <summary>
	/// オーディオを纏めれるサブミックスボイスの生成
	/// エフェクトを加えたりも出来る
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
