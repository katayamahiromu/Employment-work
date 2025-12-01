#include"ProceduralAudio.h"
#include"Audio/AudioManager.h"
#include"Audio/Oscillator.h"

ProceduralAudio::ProceduralAudio(int maxSourceCount) : maxSourceCount(maxSourceCount)
{
	signal = std::make_unique<SignalProcesser>();
	audioData.resize(maxSourceCount);
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

void ProceduralAudio::update(float elapsedTime)
{

}

void ProceduralAudio::OnGUI()
{

}

void ProceduralAudio::play(int index)
{
	//どれかのデータが使用中なら次へ
	if (audioData.at(index).isPlay())return;

	//再生させたらすぐに抜ける
	audioData.at(index).play(*signal.get());
}

void ProceduralAudio::createModalWave(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain)
{
	signal->clear();
	std::vector<uint8_t>data;
	data = Oscillator::instance()->impactModes(modes, modeCount, durationSeconds, masterGain);
	signal->addWave(data);
	signal->trySingleWave(0);
}