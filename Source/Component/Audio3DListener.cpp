#include"Audio3DListener.h"
#include"Audio/AudioManager.h"
#include"Camera.h"

Audio3DListener::Audio3DListener(int slot)
{
	AudioManager::instance()->registerAudio(&listener, slot);
}

Audio3DListener::~Audio3DListener()
{

}

void Audio3DListener::prepare()
{
	//‚Ð‚Æ‚Ü‚¸‚Ì‰Šú‰»
	listener.innerRadius = 0.7f;
	listener.outerRadius = 1.67f;
	listener.filterParam = 0.8f;
	listener.velocity = { 0.0f,0.0f,0.0f };
}

void Audio3DListener::update(float elapsedTime)
{
	listener.position = *getObject()->getPosition();
	listener.frontVec = *Camera::instance()->getFront();
	listener.rightVec = *Camera::instance()->getRight();
}

void Audio3DListener::OnGUI()
{

}