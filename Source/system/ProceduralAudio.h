#pragma once
#include"Audio/SignalProcessor.h"
#include"Audio/ModalMode.h"
#include"Audio/AudioCallback.h"
#include"Audio/AudioManager.h"
#include"Audio.h"
#include<memory>
#include<functional>

class ProceduralAudio
{
public:
	using function = std::function<std::vector<UINT8>()>;

	ProceduralAudio(int maxSourceCount);
	~ProceduralAudio();

	void play(int index,bool tryS = true);
    void stop();
	void erase(int index);
	bool isPlay();
	void pan(float pan,float frontBack);

    using CreateFunc = std::function<std::vector<uint8_t>()>;

    template<class F>
    void setCreateFunc(F&& f) {
        createFunc = std::forward<F>(f);
    }

    void initCreate(float frequency, float gain);
    void update(float frequency, float gain,bool tryS = true);

	void createModalWave(const ModalMode* modes, size_t modeCount, float durationSeconds, float masterGain);
	void loadModalData(const char* filename, float durationSeconds, float masterGain);

	SignalProcesser* getSignalProcesser(){ return signal.get(); }
private:
	std::unique_ptr<SignalProcesser> signal;
    std::unique_ptr<Audio>source;
    std::mutex mutex;
	int maxSourceCount;
    //更新用
    void create(float frequency,float gain,int index);
    CreateFunc createFunc;
    int playIndex = 0;  //生成番号
    int genIndex = 1;   //再生番号
};