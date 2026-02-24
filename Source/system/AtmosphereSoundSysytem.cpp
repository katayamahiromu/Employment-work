#include"AtmosphereSoundSysytem.h"
#include"Audio/Oscillator.h"
#include"Audio/SignalMixer.h"
#include"Audio/WaveShaper.h"

AtmosphereSound::AtmosphereSound()
{
	noise = std::make_unique<ProceduralAudio>(1);

	//‘å‹C‚ÌƒmƒCƒY
	SignalProcesser* sp = noise->getSignalProcesser();
	noise->setCreateFunc([this]() {return this->noiseCreate();});
	noise->initCreate(0.0f, 1.0f);
}

AtmosphereSound::~AtmosphereSound()
{

}

void AtmosphereSound::start()
{
	noise->play(0);
}

void AtmosphereSound::stop()
{

}

void AtmosphereSound::update()
{
	noise->update(0.0f, 1.0f);
}

void AtmosphereSound::gui()
{

}

std::vector<uint8_t> AtmosphereSound::noiseCreate()
{
	SignalProcesser* sp = noise->getSignalProcesser();
	waveData sample = sp->createData(Oscillator::instance()->brownNoise(generateInterval, SamplingRate,1.0f));
	waveData am = sp->createData(SignalMixer::instance()->amplitudeModulation(sample, 0.1f, 0.03f, 0.05f));
	return WaveShaper::instance()->LowPass(am, 300.0f, 1.0f);
}