#include"RiverSoundSystem.h"
#include"Graphics/GraphicsManager.h"
#include"math/Mathf.h"
#include"Audio/AudioManager.h"
#include"../Macro.h"
#include"Audio/PoliLineEmitter.h"

#include"Audio/Oscillator.h"
#include"Audio/SignalMixer.h"
#include"Audio/WaveShaper.h"

#include"imgui.h"

RiverSoundSystem::RiverSoundSystem()
{
	//ポリライン音源の設定
	std::shared_ptr<PoliLine>poliLine = std::make_shared<PoliLine>(pts);
	emitter.minDistance = 3.0f;
	emitter.maxDistance = 10.0f;

	//音源の設定
	noise = std::make_unique<ProceduralAudio>(1,poliLine,&emitter);
	noise->setCreateFunc([this]() {return this->createWave();});
	noise->initCreate(0.0f, 1.0f);
	noise->getAudio()->setVolume(0.5f);
}

RiverSoundSystem::~RiverSoundSystem()
{

}

void RiverSoundSystem::update()
{
	SoundListner lister = *AudioManager::instance()->findAudio(static_cast<int>(Lisner::PLAYER));
	noise->getAudio()->update3D(lister);
	noise->update(0.0f, 1.0f);
}

std::vector<uint8_t> RiverSoundSystem::createWave()
{
	auto signalProcess = noise->getSignalProcesser();
	waveData pinkNoise = signalProcess->createData(Oscillator::instance()->pinkNoise(1.0f, 44100, 0.1f), 440.0f, 0.0f);
	waveData bpf = signalProcess->createData(WaveShaper::instance()->BandPass(pinkNoise, 1000.0f, 1500.0f));
	return SignalMixer::instance()->vibratoLFO(bpf, 0.12f, 0.6f, 0.01f);
}

void RiverSoundSystem::start()
{
	noise->play(0);
}

void RiverSoundSystem::debugRender()
{
	//簡易的なデバック用の線
	DirectX::XMFLOAT4 color = { 0.0f,0.0f,0.0f,0.0f };

	GraphicsManager* graphics = GraphicsManager::instance();
	const int SUBDIV = 20; // 分割数

	for (size_t i = 1; i < pts.size() - 2; ++i)
	{
		for (int s = 0; s < SUBDIV; ++s)
		{
			float t0 = (float)s / SUBDIV;
			float t1 = (float)(s + 1) / SUBDIV;

			DirectX::XMFLOAT3 p0 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t0);
			DirectX::XMFLOAT3 p1 = Mathf::CatmullRom(pts[i - 1], pts[i], pts[i + 1], pts[i + 2], t1);

			graphics->getLineRenderer()->addVertex(p0, color);
			graphics->getLineRenderer()->addVertex(p1, color);
		}
	}

	//球体
	graphics->getDebugRenderer()->drawSphere(dynamic_cast<Audio3D*>(noise->getAudio())->getEmitterPos(), 0.5f, {1.0f,0.0f,0.0f,1.0f});
}

void RiverSoundSystem::gui()
{
	ImGui::Begin("Rever sound");
	dynamic_cast<Audio3D*>(noise->getAudio())->gui();
	ImGui::End();
}