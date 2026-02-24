#pragma once

#include <xaudio2.h>
#include "Audio.h"
#include "Audio3DSystem.h"
#include "SubMixVoiceManager.h"
#include <unordered_map>
#include"AudioWorker.h"
#include"../Utils/RingBuffer.h"

// オーディオ管理
class AudioManager
{
private:
	AudioManager() {};
	~AudioManager();

public:
	static AudioManager* instance()
	{
		static AudioManager inst;
		return &inst;
	}

	AudioManager* initialize();

	void update(float elapsedTime);

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="filename">読み込むファイル名</param>
	/// <returns>作成したオーディオ</returns>
	std::unique_ptr<Audio> loadAudioSource(const char* filename);
	std::unique_ptr<Audio> loadAudioSource(SignalProcesser&signal);
	std::unique_ptr<Audio> loadAudioSourceEffect(const char* filename);

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

	IXAudio2* getIXAudio2() { return xaudio; }

	SubMixVoiceManager* getSmv() { return smv; }

	void RegisterAudioArray(Audio* audio) { audioSources.emplace_back(audio); }

	int PlayAudioCount() { return static_cast<int>(audioSources.size()); }

	//別スレッドで計算によるデータ作成
	void CreateWaveData(AudioWorker::Task task) { worker->PushTask(task); }
	int taskCount() { return worker->TaskCount(); }
	void Gui();
private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;

	//エフェクトの設定を纏めたやつ
	SubMixVoiceManager*smv = nullptr;

	//登録
	std::unordered_map<int, SoundListner*>listenerArray;

	std::vector<Audio*>audioSources;

	//破棄リスト
	std::vector<Audio*>removeSources;

	std::unique_ptr<AudioWorker>worker;
};
