#include"windowSoundSystem.h"
#include"Audio/WaveShaper.h"
#include"Audio/Oscillator.h"
#include"Audio/SignalMixer.h"
#include"Audio/AudioManager.h"
#include"imgui.h"

WindowSoundSystem::WindowSoundSystem()
{
    windowSound = std::make_unique<ProceduralAudio>(2);
    windowSound->setCreateFunc([this]() {return this->create();});
    windowSound->initCreate(0.0f, 1.0f);
}

WindowSoundSystem::~WindowSoundSystem()
{

}

void WindowSoundSystem::start()
{
    windowSound->play(0);
}

void WindowSoundSystem::stop()
{

}

std::vector<uint8_t> WindowSoundSystem::create()
{
    auto samples = Oscillator::instance()->turbulenceNoiseSIMD(generateInterval, SamplingRate, windowSpeed, gustAmount, brightness);
    waveData data = windowSound->getSignalProcesser()->createData(samples);
    return  WaveShaper::instance()->WindHissSIMD(data, St, D, U0, rQ, windRange);
}

void WindowSoundSystem::calcPan(Camera& camera)
{
    // 風が吹いてくる方向
    DirectX::XMVECTOR windDir = DirectX::XMVector3Normalize(DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));

    // カメラの rightとfront を取得
    DirectX::XMVECTOR camRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(camera.getRight()));
    DirectX::XMVECTOR camFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(camera.getFront()));

    float pan = DirectX::XMVectorGetX(DirectX::XMVector3Dot(camRight, DirectX::XMVectorNegate(windDir)));
    pan = std::clamp(pan, -1.0f, 1.0f);

    float fb = DirectX::XMVectorGetX(DirectX::XMVector3Dot(camFront, DirectX::XMVectorNegate(windDir)));
    float frontBack = (1.0f - fb) * 0.5f;
    frontBack = std::clamp(frontBack, 0.0f, 1.0f);

    this->pan = pan;
    this->frontBack = frontBack;
}

void WindowSoundSystem::update()
{
    windowSound->pan(pan,frontBack);
    windowSound->update(0.0f, 1.0f);
}

void WindowSoundSystem::gui()
{
    ImGui::Begin("Window Sound System");
    ImGui::SliderFloat("playerSpeed", &windowSpeed, 0.0f, 1.0f);

    ImGui::SliderFloat("U0 (Wind Speed m/s)",&U0,0.0f, 60.0f, "%.1f m/s");

    ImGui::SliderFloat("pan", &pan, -1.0f, 1.0f);
    ImGui::SliderFloat("frontBack", &frontBack, 0.0f, 1.0f);
    ImGui::End();
}