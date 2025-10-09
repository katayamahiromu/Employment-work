#include "../Misc.h"
#include "AudioManager.h"

// デストラクタ
AudioManager::~AudioManager()
{
	// マスタリングボイス破棄
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio終了化
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM終了化
	CoUninitialize();
}

AudioManager* AudioManager::initialize()
{
	HRESULT hr;

	// COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// マスタリングボイス生成
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return this;
}

// オーディオソース読み込み
std::unique_ptr<Audio> AudioManager::loadAudioSource(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio>(xaudio, resource);
}

Audio*AudioManager::AllLoadAudioSource(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return new Audio(xaudio, resource);
}


std::unique_ptr<Audio3D> AudioManager::loadAudioSource3D(const char* filename, SoundEmitter* emitter)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio3D>(xaudio, resource, emitter);
}

std::unique_ptr<SubMixVoice>AudioManager::createSubMixVoice()
{
	return std::make_unique<SubMixVoice>(xaudio);
}
