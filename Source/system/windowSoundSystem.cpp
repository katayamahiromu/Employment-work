#include"windowSoundSystem.h"
#include"Audio/WaveShaper.h"
#include"Audio/Oscillator.h"
#include"Audio/AudioManager.h"
#include"imgui.h"

WindowSoundSystem::WindowSoundSystem()
{
    windowSound = std::make_unique<ProceduralAudio>(2);

    //順次処理で風の生成
    create(playIndex);

    //生成用のスレッドで生成
    AudioManager::instance()->CreateWaveData([&]() {create(genIndex);});
}

WindowSoundSystem::~WindowSoundSystem()
{

}

void WindowSoundSystem::start()
{
    windowSound->play(playIndex);
}

void WindowSoundSystem::stop()
{

}

void WindowSoundSystem::create(int index)
{
    auto samples = Oscillator::instance()->turbulenceNoiseSIMD(generateInterval, SamplingRate, windowSpeed, gustAmount, brightness);
    waveData data = windowSound->getSignalProcesser()->createData(samples);
    samples = WaveShaper::instance()->WindHissSIMD(data,St,D,U0,rQ,windRange);
    windowSound->getSignalProcesser()->addWave(samples, SamplingRate, 1.0f, index);
}

void WindowSoundSystem::update()
{
    windowSound->pan(pan,frontBack);
    if (!windowSound->isPlay(playIndex))
    {
        // 再生終了 → スワップ
        std::swap(playIndex, genIndex);

        // 再生開始
        windowSound->play(playIndex);

        //風の音の生成命令
        AudioManager::instance()->CreateWaveData([&]() {create(genIndex);});}
}

void WindowSoundSystem::gui()
{
    ImGui::Begin("Window Sound System");
    ImGui::SliderFloat("playerSpeed", &windowSpeed, 0.0f, 1.0f);

    ImGui::SliderFloat("U0 (Wind Speed m/s)",
        &U0,
        0.0f, 60.0f, "%.1f m/s");

    ImGui::SliderFloat("pan", &pan, -1.0f, 1.0f);
    ImGui::SliderFloat("frontBack", &frontBack, 0.0f, 1.0f);
    ImGui::End();
}