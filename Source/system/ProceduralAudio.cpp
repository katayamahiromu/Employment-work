#include"ProceduralAudio.h"
#include"Audio/AudioManager.h"
#include"Audio/Oscillator.h"
#include"Audio/SoundHealper.h"

ProceduralAudio::ProceduralAudio(int maxSourceCount) : maxSourceCount(maxSourceCount)
{
	signal = std::make_unique<SignalProcesser>();
	signal->resize(maxSourceCount);
	audioData.resize(1);
	for (auto& data : audioData)
	{
		data.create(AudioManager::instance()->getIXAudio2(), *signal.get());
	}
}

ProceduralAudio::~ProceduralAudio()
{
	for (auto& data : audioData)
	{
		data.destroy();
	}
}

void ProceduralAudio::play(int index)
{
	//どれかのデータが使用中なら次へ
	if (audioData.at(0).isPlay())return;

	signal->trySingleWave(index);

	//再生させたらすぐに抜ける
	audioData.at(0).play(*signal.get());
}

void ProceduralAudio::erase(int index)
{
	signal->erase(index);
}

bool ProceduralAudio::isPlay(int index)
{
	return audioData.at(0).isPlay();
}

void ProceduralAudio::createModalWave(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain)
{
	signal->clear();
	std::vector<uint8_t>data;
	data = Oscillator::instance()->impactModes(modes, modeCount, durationSeconds, masterGain);
	signal->addWave(data);
}

void ProceduralAudio::loadModalData(const char* filename ,float durationSeconds, float masterGain)
{
	std::vector<ModalMode>modals = loadModalDataJson(filename);
	createModalWave(modals.data(), modals.size(), durationSeconds, masterGain);
}

void ProceduralAudio::registerWaveData(function f,int index)
{
	signal->addWave(f(),SamplingRate,1.0f,index);
}