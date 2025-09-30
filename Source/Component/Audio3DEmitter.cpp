#include"Audio3DEmitter.h"
#include"Audio/AudioManager.h"

Audio3DEmitter::Audio3DEmitter(float minDis, float maxDis)
{
	emitter.maxDistance = maxDis;
	emitter.minDistance = minDis;
}

Audio3DEmitter::~Audio3DEmitter()
{

}

void Audio3DEmitter::prepare()
{

}

void Audio3DEmitter::update(float elapsedTime)
{
	emitter.position = *getObject()->getPosition();
}

void Audio3DEmitter::OnGUI()
{
	source->gui();
}

void Audio3DEmitter::loadFile(const char* filename)
{
	source = AudioManager::instance()->loadAudioSource3D(filename, &emitter);
}

void Audio3DEmitter::DSPSetting(int slot)
{
	source->setDSPSetting(*AudioManager::instance()->findAudio(slot));
}

void Audio3DEmitter::play(int slot)
{
	source->setDSPSetting(*AudioManager::instance()->findAudio(slot));
	source->update(1.0f);
	source->play(false);
}