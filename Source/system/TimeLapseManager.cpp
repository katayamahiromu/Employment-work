#include"TimeLapseManager.h"

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