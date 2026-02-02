#include"AudioDemoScene.h"
#include"imgui.h"
#include"Audio/Oscillator.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"../AudioFile/AudioFile.h"
#include"Audio/SoundHealper.h"
#include"SceneManager.h"
#include"LoadingScene.h"
#include"GameScene.h"
#include"../Utils/FunctionTime.h"
#include"Audio/WaveShaper.h"

extern "C" {
#include"../Utils/libtinyfiledialogs-master/tinyfiledialogs.h"
}

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
	loadPreset("preset1.json");
	loadPreset("preset2.json");
	loadPreset("preset3.json");
	modals = modalPresets.at(0).modals;


	/*auto [samples, elapsedMs] = MeasureExecutionTimeWithResult([&]() {
		return Oscillator::instance()->triangleWave(440, 1.0f);
		});
	exitTimes[0] = static_cast<float>(elapsedMs);
	signalProcess->addWave(samples, 1.0f, 44100);

	auto [samplesSIMD, elapsedMsSIMD] = MeasureExecutionTimeWithResult([&]() {
		return Oscillator::instance()->triangleWaveSIMD(440, 1.0f);
		});
	exitTimes[1] = static_cast<float>(elapsedMsSIMD);
	signalProcess->addWave(samplesSIMD, 1.0f, 44100);*/

	//signalProcess->addWave(Oscillator::instance()->whiteNoise(1.0f));

	signalProcess->addWave(Oscillator::instance()->turbulenceNoiseSIMD(1.0f, 44100, 0.5f, 0.6f, 1.0f));
	AudioManager::instance()->CreateWaveData(
		[&] {signalProcess->addWave(
			WaveShaper::instance()->WindHissSIMD(signalProcess->getWData(0),
			0.2f,      // St
			0.005f,    // D（5mm）
			25.0f,     // U0
			0.985f,    // rQ
			30.0f));}// windRange
	);

	AudioManager::instance()->CreateWaveData(
		[&]() {
			auto samples = Oscillator::instance()->turbulenceNoiseSIMD(1.0f, 44100, 0.5f, 0.6f, 1.0f);
			waveData data  = signalProcess->createData(samples);
			samples = WaveShaper::instance()->WindHissSIMD(
				data,
				0.2f,
				0.005f,
				25.0f,
				0.985f,
				30.0f);
			signalProcess->addWave(samples,44100.0f,1.0f,0);
		}
	);

	//signalProcess->addWave(WaveShaper::instance()->WindHiss(signalProcess->getWData(0),
	//	0.2f,
	//	0.005f,    // D（5mm）
	//	25.0f,     // U0
	//	0.985f,    // rQ
	//	30.0f
	//));

	//auto [samples, elapsedMs] = MeasureExecutionTimeWithResult([&]() {
	//	return WaveShaper::instance()->WindHiss(signalProcess->getWData(0),
	//		0.2f,
	//		0.005f,    // D（5mm）
	//		25.0f,     // U0
	//		0.985f,    // rQ
	//		30.0f
	//		);
	//	});
	//exitTimes[0] = static_cast<float>(elapsedMs);
	//signalProcess->addWave(samples, 1.0f, 44100);

	//auto [samplesSIMD, elapsedMsSIMD] = MeasureExecutionTimeWithResult([&]() {
	//	return WaveShaper::instance()->WindHissSIMD(signalProcess->getWData(0),
	//		0.2f,
	//		0.005f,    // D（5mm）
	//		25.0f,     // U0
	//		0.985f,    // rQ
	//		30.0f
	//		);
	//	});
	//exitTimes[1] = static_cast<float>(elapsedMsSIMD);

	//signalProcess->addWave(WaveShaper::instance()->WindHiss(signalProcess->getWData(0),
	//	0.2f,
	//	0.03f,     // D（3cm）
	//	12.0f,     // U0（ゆっくり）
	//	0.970f,    // rQ（太い）
	//	5.0f       // windRange
	//));

	//帯域事に分けてLowPassをかける
	/*signalProcess->BandPass(0, 800.0f, 3000.0f);
	signalProcess->LowPass(2, 3500.0f, 0.0f);
	signalProcess->BandPass(0, 3000.0f, 7000.0f);
	signalProcess->LowPass(3, 7000.0f, 0.0f);*/
}

void AudioDemoScene::finalize()
{

}

void AudioDemoScene::update(float elapsedTime)
{

}

void AudioDemoScene::render()
{
	DeviceManager::instance()->settingRender();
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

	ImGui::SliderFloat("frequency", &frequency, 20.0f, 1000.0f);
	ImGui::SliderFloat("durationSeconds", &durationSeconds, 0.1f, 2.0f);
	ImGui::SliderFloat("gain", &gain, 0.0f, 1.0f);
	ImGui::SliderFloat("modulationDepth", &modulationDepth, 0.0f, 1.0f);
	ImGui::SliderFloat("Decay", &decay, 0.0f, 5.0f);
	int current = static_cast<int>(uiState);
	if (ImGui::Combo("Wave Type", &current, WaveTypeNames, IM_ARRAYSIZE(WaveTypeNames))) {
		uiState = static_cast<WaveType>(current);
	}

	if (ImGui::Button("create wave data"))
	{
		std::vector<uint8_t>data;
		switch (uiState)
		{
		case WaveType::Sine:
			data = Oscillator::instance()->sinWaveSIMD(frequency, durationSeconds);
			//signalProcess->setWaveTypeData(WaveTypeInfo::Sin, frequency, durationSeconds);
			break;
		case WaveType::Saw:
			data = Oscillator::instance()->sawtoothWaveSIMD(frequency, durationSeconds);
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

		//レンダリングした波形データを追加
		signalProcess->addWave(data, frequency, gain);
	}

	std::vector<const char*> names;
	for (auto& p : modalPresets) names.push_back(p.name.c_str());
	if (ImGui::Combo("Modal Type", &modalCurrent, names.data(), names.size()))
	{
		modals = modalPresets.at(modalCurrent).modals;
	}

	if (ImGui::Button("modal wave"))
	{
		std::vector<uint8_t>data;
		data = Oscillator::instance()->impactModes(modals.data(), modals.size(), durationSeconds, gain);
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

	ImGui::Begin("data managment");
	if (ImGui::Button("Export Wave file"))
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
		std::string file = modalPresets.at(modalCurrent).name + extension;

		// すでに存在するかチェック
		if (!std::filesystem::exists(file)) {
			audio.save(file);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Export Json"))
	{

		nlohmann::json root;
		nlohmann::json modeArray = nlohmann::json::array();
		for (const auto& m : modals)
		{
			nlohmann::json jm;
			toModalJson(jm, m);
			modeArray.emplace_back(jm);
		}
		root["modals"] = modeArray;

		std::string extension = ".json";
		std::string file = modalPresets.at(modalCurrent).name + extension;

		if (!std::filesystem::exists(file))
		{
			std::ofstream ofs(file);
			ofs << root.dump(4);
		}
	}

	
	if (ImGui::Button("load josn data"))
	{
		const char* filePath = tinyfd_openFileDialog(
			"Select jsonFile",
			"",
			0,
			nullptr,
			nullptr,
			0
		);

		if (filePath) {
			std::string pathStr = filePath;

			// 拡張子チェック（小文字化して比較）
			std::string lower = pathStr;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

			if (lower.size() >= 5 && lower.substr(lower.size() - 5) == ".json") {
				loadPreset(filePath);
			}
		}
	}

	ImGui::SameLine();
	ImGui::End();

	ImGui::Begin("Use Procedural Audio Scene");
	if (ImGui::Button("Change Scene"))
	{
		SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
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
	ImGui::InputText("new create modals name", fileName, IM_ARRAYSIZE(fileName));
	if (ImGui::Button("new create modals"))
	{
		if (fileName[0] != '\0')
		{
			preset p;
			p.name = fileName;
			modalPresets.emplace_back(p);
			modalCurrent = static_cast<int>(modalPresets.size()-1);
			modals = modalPresets.at(modalCurrent).modals;
			fileName[0] = '\0';
		}
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

void AudioDemoScene::loadPreset(const char* filePath)
{
	preset p;
	p.modals = loadModalDataJson(filePath);
	// ファイル名からプリセット名を生成
	std::string filename = filePath;
	size_t lastSlash = filename.find_last_of("/\\");
	if (lastSlash != std::string::npos)
		filename = filename.substr(lastSlash + 1);

	size_t extPos = filename.rfind(".json");
	if (extPos != std::string::npos)
		filename = filename.substr(0, extPos);

	p.name = filename;

	modalPresets.push_back(std::move(p));
}