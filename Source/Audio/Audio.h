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

// オーディオ
class Audio
{
public:
	Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~Audio();

	/// <summary>
	/// オーディオの再生
	/// </summary>
	/// <param name="loop">true...ループ</param>
	void play(bool loop);

	/// <summary>
	/// オーディオの停止
	/// </summary>
	void stop();

	/// <summary>
	/// ワンショットのオーディオ再生 
	/// </summary>
	void DCPlay();

	/// <summary>
	/// 途中から再生
	/// </summary>
	void middlePlay(float time);

	/// <summary>
	/// 指定した所をループ
	/// </summary>
	void sustainLoop(float loop_start, float loop_end);

	/// <summary>
	/// ボリュームの変更
	/// </summary>
	void setVolume(float volum);

	/// <summary>
	/// ピッチの変更
	/// </summary>
	void setPitch(float pitch);

	/// <summary>
	/// パニング
	/// </summary>
	void setPan(float pan);

	/// <summary>
	/// ピッチのリセット
	/// </summary>
	void resetPitch();

	/// <summary>
	/// 一時停止
	/// </summary>
	void pause();

	/// <summary>
	/// リスタート
	/// </summary>
	void reStart();

	/// <summary>
	/// フィルターの設定
	/// </summary>
	void setFilter(int type);

	//再生中か
	bool isPlay();

	/// <summary>
	/// 出力先をサブミックスボイスに設定する
	/// </summary>
	void setSubmixVoice(SubMixVoice*sv);
protected:
	IXAudio2SourceVoice* sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
	float volume = 1.0f;
};
