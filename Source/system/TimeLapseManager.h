#pragma once
#include"Component/TimeLapse.h"
#include"../Utils/Singleton.h"
#include<list>

class TimeLapseManager
{
public:
	TimeLapseManager();
	~TimeLapseManager();

	static TimeLapseManager& instance()
	{
		static TimeLapseManager instance;
		return instance;
	}

	void regist(TimeLapse*timelapse) { timeLapsArray.push_back(timelapse); };

	void outputRecordInformation();

	bool getIsRecord() { return isRecord; }
	void setIsRecord(bool setting) { isRecord = setting; }

	void setIsPushButton(bool setting) { isPushButton = setting; }
	void setIsRelease(bool setting) { isReleaseButton = setting; }
	void update();
private:
	bool isRecord = false;
	bool isPushButton = false;
	bool isReleaseButton = false;
	bool reverse = true;
	std::list<TimeLapse*>timeLapsArray;
};