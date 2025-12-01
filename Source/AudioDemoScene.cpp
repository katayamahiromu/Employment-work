#include"AudioDemoScene.h"
#include"imgui.h"
#include"Audio/Oscillator.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"../AudioFile/AudioFile.h"

AudioDemoScene::AudioDemoScene()
{

}

AudioDemoScene::~AudioDemoScene()
{

}

void AudioDemoScene::initialize()
{
	signalProcess = std::make_unique<SignalProcesser>();
	IXAudio2* audio = AudioManager::instance()->getIXAudio2();
	audio->CreateSourceVoice(&source, &signalProcess->getWaveFormat());
}

void AudioDemoScene::finalize()
{

}

void AudioDemoScene::update(float elapsedTime)
{

}

void AudioDemoScene::toJson(json& j, const ModalMode& m)
{
	j = json
	{
		{"frequency", m.frequency},
		{"amplitude", m.amplitude},
		{"decayTime", m.decayTime},
		{"phase", m.phase},
		{"startSec", m.startSec},
		{"gain", m.gain},
		{"Bandwidth", m.bandwidth},
		{"Inharmonicity", m.inharmonicity},
		{"Noise Mix", m.noiseMix},
		{"Harmonic Mask", m.harmonicMask},
		{"Random Phase", m.randomPhase},
		{"Random Decay", m.randomDecay},
		{"Clip Amount", m.clipAmount}
	};
}

void AudioDemoScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	mgr->settingRender();

	gui();
}

void AudioDemoScene::gui()
{
	ProceduralAudioGui();
	importData();
	inputModalGui();
}

void AudioDemoScene::ProceduralAudioGui()
{
	ImGui::Begin("Procedural Audio");

	auto play = [&]()
		{
			XAUDIO2_BUFFER buffer{};
			buffer.AudioBytes = signalProcess->getAudioBytes();
			buffer.pAudioData = signalProcess->getAudioData();
			source->SubmitSourceBuffer(&buffer);
			source->Start();
		};

	static const char* WaveTypeNames[] = {
		"Sine", "Saw", "Triangle", "Square", "Noise", "Impact"
	};

	static const char* modalPreset[] = {
		"Preset1", "Preset2", "Preset3","create",
	};

	ImGui::SliderFloat("frequency", &frequency, 0.0f, 1000.0f);
	ImGui::SliderFloat("durationSeconds", &durationSeconds, 0.1f, 2.0f);
	ImGui::SliderFloat("gain", &gain, 0.0f, 1.0f);
	ImGui::SliderFloat("modulationDepth", &modulationDepth, 0.0f, 1.0f);
	ImGui::SliderFloat("Decay", &decay, 0.0f, 5.0f);
	int current = static_cast<int>(uiState);
	if (ImGui::Combo("Wave Type", &current, WaveTypeNames, IM_ARRAYSIZE(WaveTypeNames))) {
		uiState = static_cast<WaveType>(current);
	}

	int modalCurrent = static_cast<int>(modalState);
	if (ImGui::Combo("Modal Type", &modalCurrent, modalPreset, IM_ARRAYSIZE(modalPreset)))
	{
		modalState = static_cast<modalTyepe>(modalCurrent);
	}

	if (ImGui::Button("create wave data"))
	{
		std::vector<uint8_t>data;
		switch (uiState)
		{
		case WaveType::Sine:
			data = Oscillator::instance()->sinWave(frequency, durationSeconds);
			break;
		case WaveType::Saw:
			data = Oscillator::instance()->sawtoothWave(frequency, durationSeconds);
			break;
		case WaveType::Triangle:
			data = Oscillator::instance()->triangleWave(frequency, durationSeconds);
			break;
		case WaveType::Square:
			data = Oscillator::instance()->squareWave(frequency, durationSeconds);
			break;
		case WaveType::Noise:
			data = Oscillator::instance()->whiteNoise(durationSeconds);
			break;
		case WaveType::Impact:
			data = Oscillator::instance()->impactSound(0.8f, durationSeconds);
			break;
		default:
			break;
		}
		signalProcess->addWave(data, frequency, gain);
	}
	ImGui::SameLine();

	if (ImGui::Button("modal wave"))
	{
		std::vector<uint8_t>data;
		switch (modalState)
		{
		case modalTyepe::preset1:
			data = Oscillator::instance()->impactModes(stoneModes, 5, durationSeconds, gain);
			break;
		case modalTyepe::preset2:
			data = Oscillator::instance()->impactModes(leatherShoeModes, 5, durationSeconds, gain);
			break;
		case modalTyepe::preset3:
			data = Oscillator::instance()->impactModes(caveRockModes, 4, durationSeconds, gain);
			break;
		case  modalTyepe::create:
			data = Oscillator::instance()->impactModes(modals.data(), modals.size(), durationSeconds, gain);
			break;
		default:
			break;
		}
		signalProcess->addWave(data, frequency, gain);
	}

	if (ImGui::Button("Clear"))
	{
		signalProcess->clear();
	}

	if (ImGui::Button("all mix sound play"))
	{
		signalProcess->Mix();
		play();
	}

	int size = signalProcess->size();
	ImGui::InputInt("wave count", &size);

	int id = 0;
	std::vector<int>eraseArray;
	for (int i = 0;i < size;++i)
	{
		ImGui::PushID(id);
		if (ImGui::Button("play id"))
		{
			signalProcess->trySingleWave(id);
			play();
		}
		ImGui::SameLine();
		if (ImGui::Button("erase"))
		{
			eraseArray.emplace_back(id);
		}
		ImGui::SameLine();
		if (ImGui::Button("Decay"))
		{
			signalProcess->decayWave(id, decay);
		}
		ImGui::Separator();
		ImGui::PopID();
		id++;
	}

	for (auto erase : eraseArray)signalProcess->erase(erase);
	eraseArray.clear();

	ImGui::SliderInt("carrierIndex", &carrierIndex, 0, size - 1);
	ImGui::SliderInt("modIndex", &modIndex, 0, size - 1);
	if (ImGui::Button("FM"))
	{
		signalProcess->applyFM(carrierIndex, modIndex, modulationDepth, gain);
		play();
	}
	ImGui::SameLine();
	if (ImGui::Button("AM"))
	{
		signalProcess->applyAM(carrierIndex, modIndex, modulationDepth, gain);
		play();
	}
	ImGui::SameLine();
	if (ImGui::Button("RM"))
	{
		signalProcess->applyRM(carrierIndex, modIndex, modulationDepth, gain);
		play();
	}

	ImGui::End();
}

void AudioDemoScene::importData()
{

	ImGui::Begin("Import");
	ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));
	if (ImGui::Button("import Wave file"))
	{
		int channels = signalProcess->getWaveFormat().nChannels;
		int samples = signalProcess->getAudioBytes() / (signalProcess->getWaveFormat().wBitsPerSample / 8) / channels;

		AudioFile<float>audio;
		audio.setAudioBufferSize(channels, samples);

		const uint8_t* data = signalProcess->getAudioData();
		for (int i = 0; i < samples; ++i)
		{
			for (int ch = 0; ch < channels; ++ch)
			{
				int index = (i * channels + ch) * 2; // 16bit = 2byte

				// little endian → int16 に変換
				int16_t sample =
					data[index] | (data[index + 1] << 8);

				// AudioFile の -1〜+1 float に変換
				audio.samples[ch][i] = sample / 32768.0f;
			}
		}

		audio.setSampleRate(44100);
		audio.setBitDepth(16);

		std::string extension = ".wav";
		std::string file = fileName + extension;
		audio.save(file);
	}

	ImGui::SameLine();

	if (ImGui::Button("Import Json"))
	{

		json root;
		json modeArray = json::array();
		for (const auto& m : modals)
		{
			json jm;
			toJson(jm, m);
			modeArray.emplace_back(jm);
		}
		root["modals"] = modeArray;

		std::string extension = ".json";
		std::string file = fileName + extension;

		std::ofstream ofs(file);
		ofs << root.dump(4);
	}
	ImGui::End();
}

void AudioDemoScene::inputModalGui()
{
	ImGui::Begin("add modal data");
	static ModalMode temp = {
		440.0f,  // frequency
		1.0f,    // amplitude
		0.5f,    // decayTime
		0.0f,    // phase
		0.0f,    // startSec
		1.0f,    // gain
		50.0f,   // bandwidth
		0.0f,    // inharmonicity
		0.0f,    // noiseMix
		1.0f,    // harmonicMask
		0.0f,    // randomPhase
		0.0f,    // randomDecay
		0.0f,    // clipAmount
		20000.0f,// lowpassCutoff
		20.0f    // highpassCutoff
	};

	ImGui::InputFloat("Frequency (Hz)", &temp.frequency);
	ImGui::InputFloat("Amplitude", &temp.amplitude);
	ImGui::InputFloat("Decay Time (s)", &temp.decayTime);
	ImGui::InputFloat("Phase (rad)", &temp.phase);
	ImGui::InputFloat("Start Time (s)", &temp.startSec);
	ImGui::InputFloat("Gain", &temp.gain);
	ImGui::InputFloat("Bandwidth", &temp.bandwidth);
	ImGui::InputFloat("Inharmonicity", &temp.inharmonicity);
	ImGui::InputFloat("Noise Mix", &temp.noiseMix);
	ImGui::InputFloat("Harmonic Mask", &temp.harmonicMask);

	ImGui::InputFloat("Random Phase", &temp.randomPhase);
	ImGui::InputFloat("Random Decay", &temp.randomDecay);
	ImGui::InputFloat("Clip Amount", &temp.clipAmount);

	ImGui::InputFloat("Lowpass Cutoff (Hz)", &temp.lowpassCutoff);
	ImGui::InputFloat("Highpass Cutoff (Hz)", &temp.highpassCutoff);



	if (ImGui::Button("add modal"))
	{
		modals.emplace_back(temp);
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		modals.clear();
	}

	ImGui::Separator();
	ImGui::Text("Modal List");
	ImGui::Separator();

	//モーダルの一覧
	for (int i = 0; i < modals.size(); ++i)
	{
		ImGui::PushID(i);

		char label[64];
		sprintf_s(label, "Modal %d", i);

		if (ImGui::TreeNode(label))
		{
			ImGui::InputFloat("Frequency (Hz)", &modals[i].frequency);
			ImGui::InputFloat("Amplitude", &modals[i].amplitude);
			ImGui::InputFloat("Decay Time (s)", &modals[i].decayTime);
			ImGui::InputFloat("Phase (rad)", &modals[i].phase);
			ImGui::InputFloat("Start Time (s)", &modals[i].startSec);
			ImGui::InputFloat("Gain", &modals[i].gain);

			ImGui::InputFloat("Bandwidth", &modals[i].bandwidth);
			ImGui::InputFloat("Inharmonicity", &modals[i].inharmonicity);
			ImGui::InputFloat("Noise Mix", &modals[i].noiseMix);
			ImGui::InputFloat("Harmonic Mask", &modals[i].harmonicMask);

			ImGui::InputFloat("Random Phase", &modals[i].randomPhase);
			ImGui::InputFloat("Random Decay", &modals[i].randomDecay);
			ImGui::InputFloat("Clip Amount", &modals[i].clipAmount);

			ImGui::InputFloat("Lowpass Cutoff (Hz)", &modals[i].lowpassCutoff);
			ImGui::InputFloat("Highpass Cutoff (Hz)", &modals[i].highpassCutoff);

			ImGui::Separator();
			// 削除ボタン
			if (ImGui::Button("Remove This Modal"))
			{
				modals.erase(modals.begin() + i);
				ImGui::TreePop();
				ImGui::PopID();
				break;
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	ImGui::End();
}