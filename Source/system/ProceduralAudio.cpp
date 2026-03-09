#include"ProceduralAudio.h"
#include"Audio/AudioManager.h"
#include"Audio/Oscillator.h"
#include"Audio/SoundHealper.h"

ProceduralAudio::ProceduralAudio(int maxSourceCount) : maxSourceCount(maxSourceCount)
{
	signal = std::make_unique<SignalProcesser>();
	signal->resize(maxSourceCount);
	source = AudioManager::instance()->loadAudioSource(*signal);
}

ProceduralAudio::ProceduralAudio(int maxSourceCount, std::shared_ptr<BaseEmitter>emitterType, SoundEmitter* emitter) : maxSourceCount(maxSourceCount)
{
	signal = std::make_unique<SignalProcesser>();
	signal->resize(maxSourceCount);
	source = AudioManager::instance()->loadAudioSource3D(*signal, emitterType, emitter);
}

ProceduralAudio::~ProceduralAudio()
{
	
}

void ProceduralAudio::play(int index, bool tryS)
{
	if (source->isPlay())return;

	if(tryS) signal->trySingleWave(index);

	source->play(false);
}

void ProceduralAudio::stop()
{
	source->stop();
}

void ProceduralAudio::erase(int index)
{
	signal->erase(index);
}

bool ProceduralAudio::isPlay()
{
	return source->isPlay();
}

void ProceduralAudio::pan(float pan, float frontBack)
{
	source->setPan(pan, frontBack);
}

void ProceduralAudio::initCreate(float frequency, float gain)
{
	create(frequency, gain,playIndex);
	AudioManager::instance()->CreateWaveData([=]() {create(frequency, gain,genIndex);});
}

void ProceduralAudio::update(float frequency, float gain,bool tryS)
{
	if (!isPlay())
	{
		std::swap(playIndex, genIndex);
		play(playIndex);

		//Ä¶Œã¶¬–½—ß‚ð”­M
		AudioManager::instance()->CreateWaveData(
			[this, frequency, gain, index = genIndex]()
			{
				create(frequency, gain, index);
			}
		);
	}
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

void ProceduralAudio::create(float frequency, float gain, int index)
{
	std::lock_guard<std::mutex> lock(mutex);
	if (createFunc)
	{
		signal->addWave(createFunc(), frequency, gain, index);
	}
}