#include"AudioCallback.h"
#include"AudioManager.h"
#include"../system/ProceduralAudio.h"

AudioCallback::AudioCallback()
{

}

AudioCallback::~AudioCallback()
{

}

void AudioCallback::OnVoiceProcessingPassEnd()noexcept
{

}

void AudioCallback::OnVoiceProcessingPassStart(UINT32 SamplesRequired) noexcept
{

}

////終端到達時
void AudioCallback::OnStreamEnd()noexcept
{

}

//バッファの再生が終了した時
void AudioCallback::OnBufferEnd(void* pBufferContext)noexcept
{
	static_cast<Audio*>(pBufferContext)->endPlay();
}

//バッファの再生が始まる直前
void AudioCallback::OnBufferStart(void* pBufferContext)noexcept
{
	Audio* audio = static_cast<Audio*>(pBufferContext);

	//既に再生フラグが立っていればリターン
	if (audio->getPlayFlag())return;
	AudioManager::instance()->RegisterAudioArray(audio);
}

//ループポイント到達時
void AudioCallback::OnLoopEnd(void* pBufferContext)noexcept
{
	static_cast<Audio*>(pBufferContext)->loopCountUp();
}

//再生が中断された時
void AudioCallback::OnVoiceError(void* pBufferContext, HRESULT Error) noexcept
{

}

//プロシージャルオーディオ用のコールバック処理
ProceduralAudioCallback::ProceduralAudioCallback()
{

}

ProceduralAudioCallback::~ProceduralAudioCallback()
{

}

void ProceduralAudioCallback::OnStreamEnd()noexcept
{

}

void ProceduralAudioCallback::OnVoiceProcessingPassEnd()noexcept
{

}

void ProceduralAudioCallback::OnVoiceProcessingPassStart(UINT32 SamplesRequired)noexcept
{

}

void ProceduralAudioCallback::OnBufferEnd(void* pBufferContext)noexcept
{
	//static_cast<AudioData*>(pBufferContext)->playing = false;
}

void ProceduralAudioCallback::OnBufferStart(void* pBufferContext)noexcept
{
	//static_cast<AudioData*>(pBufferContext)->playing = true;
}

void ProceduralAudioCallback::OnLoopEnd(void* pBufferContext)noexcept
{

}

void ProceduralAudioCallback::OnVoiceError(void* pBufferContext, HRESULT Error)noexcept
{

}