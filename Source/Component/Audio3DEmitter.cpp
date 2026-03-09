#include"Audio3DEmitter.h"
#include"Audio/AudioManager.h"
#include"Audio/PointEmitter.h"

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
	//DSP‚ÌXV
	if (listener == nullptr)return;
	source->update3D(*listener);
}

void Audio3DEmitter::OnGUI()
{
	source->gui();
}

void Audio3DEmitter::loadFile(const char* filename)
{
	std::shared_ptr<PointEmitter> point = std::make_shared<PointEmitter>(getObject()->getPPosition());
	source = AudioManager::instance()->loadAudioSource3D(filename, point, &emitter);
}

void Audio3DEmitter::play(int slot)
{
	source->play(false);
}