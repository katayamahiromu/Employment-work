#include"AuidoResourceList.h"
#include"../Utils/rapidcsv-master/src/rapidcsv.h"
#include"../Audio/AudioManager.h"
#include"../imgui/imgui.h"
#include <utility>

AudioResourceList::AudioResourceList()
{

}

AudioResourceList::~AudioResourceList()
{

}

void AudioResourceList::loadResource(const char* path, bool revers)
{
	rapidcsv::Document doc(path, rapidcsv::LabelParams(0, -1));

	std::string folderPath = "Resources\\Audio\\";
	std::string extension = ".wav";
	for (size_t i = 0;i < doc.GetRowCount();++i)
	{
		std::string filename = doc.GetCell<std::string>("wav", i);
		std::string name = doc.GetCell<std::string>("name", i);

		if (name.empty() || filename.empty()) continue;

		std::string audioPath = folderPath + filename + extension;
		AudioInfo info;
		info.source = AudioManager::instance()->loadAudioSource(audioPath.c_str());
		audioList.emplace(name,std::move(info));


		//‹tÄ¶‚à¶¬‚·‚é‚È‚ç
		if (revers)
		{
			AudioInfo info;
			std::string revers = "r";
			info.source = AudioManager::instance()->loadAudioSource(audioPath.c_str());
			info.source->createReversWav();
			audioList.emplace(revers + name, std::move(info));
		}
	}
}

void AudioResourceList::allClear()
{
	for (auto& audio : audioList)
	{
		audio.second.source->stop();
	}

	audioList.clear();
}

Audio* AudioResourceList::getAudio(std::string name)
{
	auto it = audioList.find(name);
	if (it != audioList.end())
		return it->second.source.get();

	return nullptr;
}

void AudioResourceList::onGui()
{
	ImGui::Begin("Audio Preview");
	for (auto& source : audioList)
	{
		ImGui::Text(source.second.source.get()->getName().c_str());
	}
	ImGui::End();
}