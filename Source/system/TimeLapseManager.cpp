#include"TimeLapseManager.h"
#include"ResourceList/AuidoResourceList.h"

TimeLapseManager::TimeLapseManager()
{

}

TimeLapseManager::~TimeLapseManager()
{

}

void TimeLapseManager::outputRecordInformation()
{
	for (TimeLapse* ti : timeLapsArray)
	{
		ti->outputRecordInfo();
	}
}

void TimeLapseManager::update()
{
	if (isPushButton || isReleaseButton)
	{
		reverse = !reverse;
		AudioResourceList* list = AudioResourceList::instance();
		Audio* origin = list->getAudio("bgm");
		Audio* reversOrigin = list->getAudio("rbgm");
		reverse ? origin->reversPlay(reversOrigin) : reversOrigin->reversPlay(origin);
	}
}