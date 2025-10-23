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
	//押されている時かボタンが離れた時に作動
	if (isPushButton || isReleaseButton)
	{
		reverse = !reverse;
		AudioResourceList* list = AudioResourceList::instance();
		Audio* origin = list->getAudio("bgm");
		Audio* reversOrigin = list->getAudio("rbgm");
		Audio* effect = list->getAudio("timeReep");

		if (reverse)
		{
			origin->reversPlay(reversOrigin);
			effect->stop();
		}
		else
		{
			reversOrigin->setPitch(1.5 + 0.5f * ((float)rand() / RAND_MAX));
			reversOrigin->reversPlay(origin);
			effect->setVolume(2.0f);
			effect->play(true);
		}
	}

	//ボタンが押されている時
	if (isPushButton)
	{
		frameCount--;
	}
	else
	{
		frameCount++;
	}
}