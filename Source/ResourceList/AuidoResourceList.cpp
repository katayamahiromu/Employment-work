#include"AuidoResourceList.h"
#include"../Utils/rapidcsv-master/src/rapidcsv.h"
#include"../Audio/AudioManager.h"

AudioResourceList::AudioResourceList()
{

}

AudioResourceList::~AudioResourceList()
{

}

void AudioResourceList::loadResource(char* path)
{
	rapidcsv::Document doc(path, rapidcsv::LabelParams(0, -1));

	std::string folderPath = "Resources/Audio/";
	std::string extension = ".wav";
	for (size_t i = 0;i < doc.GetRowCount();++i)
	{
		std::string filename = doc.GetCell<std::string>("wav", i);
		std::string name = doc.GetCell<std::string>("name", i);
	}
}

Audio* AudioResourceList::getAudio(char* name)
{

}