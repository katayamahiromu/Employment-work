#pragma once
#include<xaudio2.h>

class AudioCallback :public IXAudio2VoiceCallback
{
public:
	AudioCallback();
	~AudioCallback();

	//ストリームの終端に到達した時
	void OnStreamEnd()noexcept override;

	void OnVoiceProcessingPassEnd()noexcept override;
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) noexcept override;
	
	//バッファの再生が終了した時
	void OnBufferEnd(void* pBufferContext)noexcept override;
	
	//バッファの再生が始まる直前
	void OnBufferStart(void* pBufferContext)noexcept override;

	//ループポイント到達時
	void OnLoopEnd(void* pBufferContext)noexcept override;

	//再生が中断された時
	void OnVoiceError(void* pBufferContext, HRESULT Error)noexcept override;
};