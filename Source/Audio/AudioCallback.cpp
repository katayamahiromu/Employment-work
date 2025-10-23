#include"AudioCallback.h"
#include"Audio.h"

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

}

//バッファの再生が始まる直前
void AudioCallback::OnBufferStart(void* pBufferContext)noexcept
{

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