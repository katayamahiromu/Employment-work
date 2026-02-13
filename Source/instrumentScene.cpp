#include"instrumentScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"imgui.h"
#include"Audio/Oscillator.h"
#include"Audio/SignalMixer.h"
#include"Input/InputManager.h"
#include"SceneManager.h"
#include"AudioDemoScene.h"

#include"MusicData/musicData1.h"

#define Osc Oscillator::instance()
#define Mixer SignalMixer::instance()

void InstrumentScene::initialize()
{
	instrument = std::make_unique<ProceduralAudio>(1);


	//残酷な天使のテーゼ生成
	Mu = std::make_unique<ProceduralAudio>(1);

	addInstrument(Music1::NoteTable, 0);
	addInstrument(Music1::base1, 1);
	addInstrument(Music1::base2, 2);
}

void InstrumentScene::finalize()
{

}

void InstrumentScene::update(float elapsedTIme)
{
	InputManager* input = InputManager::instance();

	auto* pad = input->getGamePad();
	int buttons[nodeMax] = {
		pad->BTN_1, pad->BTN_2, pad->BTN_3, pad->BTN_4,
		pad->BTN_5, pad->BTN_6, pad->BTN_7, pad->BTN_8
	};

	for (int i = 0; i < nodeMax; ++i)
	{
		if (buttons[i] & pad->getButtonDown()) playIndex(i);
	}
}

void InstrumentScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	mgr->settingRender();

	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	gui();
}

void InstrumentScene::gui()
{
	ImGui::Begin("Sound setting");
	ImGui::InputInt("basis octave", &basisOctave);
	
	if (ImGui::Button("octave up"))
	{
		basisOctave++;
		freqMultiplier *= octaveUp;
	}
	ImGui::SameLine();
	if (ImGui::Button("octave dawn"))
	{
		basisOctave--;
		freqMultiplier *= octaveDawn;
	}

	ImGui::Text(keyArray[key].c_str());
	if (ImGui::Button("Semitone Up"))
	{
		key++;
		if (key >= 12)
		{
			key = 0;
			basisOctave++;
		}
		freqMultiplier *= SemitoneUp;
	}
	ImGui::SameLine();
	if (ImGui::Button("Semitone Down"))
	{
		key--;
		if (key < 0)
		{
			key = 11;
			basisOctave--;
		}
		freqMultiplier *= SemitoneDown;
	}

	const char* waveNames[] = { "Sine", "Saw", "Square", "Triangle" };
	int current = static_cast<int>(currentWave);
	if (ImGui::Combo("current Wave Type", &current, waveNames, IM_ARRAYSIZE(waveNames)))
	{
		currentWave = static_cast<WaveType>(current);
	}

	int mod = static_cast<int>(modWave);
	if (ImGui::Combo("mod Wave Type", &mod, waveNames, IM_ARRAYSIZE(waveNames)))
	{
		modWave = static_cast<WaveType>(mod);
	}

	const char* mixNames[] = { "FM","AM","RM" };
	int mix = static_cast<int>(mixType);
	if (ImGui::Combo("Mix Type", &mix, mixNames, IM_ARRAYSIZE(mixNames)))
	{
		mixType = static_cast<MixType>(mix);
	}

	if (ImGui::Button("music start"))
	{
		Mu->getSignalProcesser()->Mix();
		Mu->play(0,false);
	}
	ImGui::End();

	ImGui::Begin("Scene Change");
	if (ImGui::Button("Audio Demo"))
	{
		SceneManager::instance()->changeScene(new AudioDemoScene);
	}
	ImGui::End();
}

std::vector<uint8_t>InstrumentScene::generateWave(WaveType type,float freq)
{
	float phase = 0.0f;
	switch (type)
	{
	case InstrumentScene::WaveType::Sine:
		return Osc->sinWaveSIMD(freq, 0.5f, phase);
	case InstrumentScene::WaveType::Saw:
		return Osc->sawtoothWaveSIMD(freq, 0.5f);
	case InstrumentScene::WaveType::Square:
		return Osc->squareWaveSIMD(freq, 0.5);
	case InstrumentScene::WaveType::Triangle:
		return Osc->triangleWave(freq, 0.5f);
	}
	return {};
}

std::vector<UINT8>InstrumentScene::mix(MixType type, waveData& data1, waveData& data2)
{
	switch (type)
	{
	case InstrumentScene::MixType::FM:
		return Mixer->frequencyModulation(data1, data2, 1.0f);
	case InstrumentScene::MixType::RM:
		return Mixer->ringModulation(data1, data2, 1.0f);
	case InstrumentScene::MixType::AM:
		return Mixer->amplitudeModulation(data1, data2, 1.0f);
	}
	return {};
}

void InstrumentScene::playIndex(int index)
{
	//周波数の計算
	float freq = scale[index] * freqMultiplier;
	
	//カレント波形データ
	waveData data1 = instrument->getSignalProcesser()->createData(
		generateWave(currentWave, freq), freq, phase[index]
	);

	//モジュレーション波形データ
	waveData data2 = instrument->getSignalProcesser()->createData(
		generateWave(modWave, freq), freq, phase[index]
	);

	//合成
	auto sample = mix(mixType, data1, data2);

	instrument->getSignalProcesser()->addWave(sample, freq, 1.0f, index);
	instrument->play(index);
}

void InstrumentScene::addInstrument(auto data, int addIndex)
{
	std::vector<std::vector<uint8_t>>music;
	for (auto& data : data)
	{
		auto sample = Osc->squareWaveSIMD(data.freq, data.length);
		music.push_back(sample);
	}

	std::vector<uint8_t> merged;
	size_t total = 0;
	for (const auto& v : music) {
		total += v.size();
	}
	merged.reserve(total);

	// すべて結合
	for (const auto& v : music) {
		merged.insert(merged.end(), v.begin(), v.end());
	}
	Mu->getSignalProcesser()->addWave(merged, 0.0f, 1.0f, addIndex);
}