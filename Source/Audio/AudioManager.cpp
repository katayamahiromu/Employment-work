#include "../Misc.h"
#include "AudioManager.h"
#include"imgui.h"

// デストラクタ
AudioManager::~AudioManager()
{
	delete smv;

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
	createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// マスタリングボイス生成
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	smv = new SubMixVoiceManager;
	smv->createSubMixVoice();

	worker = std::make_unique<AudioWorker>();
	return this;
}

void AudioManager::update(float elapsedTime)
{
	//サブミックスボイスの更新
	smv->update();

	for (auto& audioSource : audioSources)
	{
		audioSource->update(elapsedTime);

		//破棄リストに追加
		if (audioSource->getPlayFlag())
		{
			audioSource->destruction();
			removeSources.push_back(audioSource);
		}
	}

	//破棄処理
	for (auto& audioSource : removeSources)
	{
		auto itr = std::find(audioSources.begin(), audioSources.end(), audioSource);
		if (itr != audioSources.end())
		{
			audioSources.erase(itr);
		}
	}

	removeSources.clear();
}

// オーディオソース読み込み
std::unique_ptr<Audio> AudioManager::loadAudioSource(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio>(xaudio, resource,false);
}

std::unique_ptr<Audio> AudioManager::loadAudioSource(SignalProcesser& signal)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(signal);
	return std::make_unique<Audio>(xaudio, resource, false);
}

std::unique_ptr<Audio> AudioManager::loadAudioSourceEffect(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio>(xaudio, resource, true);
}

std::unique_ptr<Audio3D> AudioManager::loadAudioSource3D(const char* filename, std::shared_ptr<BaseEmitter>emitterType,SoundEmitter* emitter)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio3D>(xaudio, resource,emitterType ,emitter);
}

std::unique_ptr<Audio3D>AudioManager::loadAudioSource3D(SignalProcesser& siganl, std::shared_ptr<BaseEmitter>emitterType,SoundEmitter* emitter)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(siganl);
	return std::make_unique<Audio3D>(xaudio, resource, emitterType, emitter);
}

std::unique_ptr<SubMixVoice>AudioManager::createSubMixVoice()
{
	return std::make_unique<SubMixVoice>(xaudio);
}

void AudioManager::Gui()
{
	ImGui::Begin("Audio Preview");
	smv->Gui();
	ImGui::End();
}