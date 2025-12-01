#include"SoundEffect.h"
#include"../imgui/imgui.h"

SoundEffect::~SoundEffect()
{
	release();
}

void SoundEffect::release()
{
	if (pXAPO) 
	{
		pXAPO->Release();
		pXAPO = nullptr;
	}
}

Reverb::Reverb(int slot):SoundEffect(slot)
{
	//XAudio2CreateReverb(&pXAPO);
	CreateFX(__uuidof(FXReverb), &pXAPO);
}

Reverb::~Reverb()
{
}

void Reverb::update(IXAudio2SubmixVoice* sv)
{
	//値が変更されてないなら即リターン
	//if (!isAlter)return;

	/*XAUDIO2FX_REVERB_I3DL2_PARAMETERS preset = XAUDIO2FX_I3DL2_PRESET_GENERIC;
	preset.WetDryMix = wetLevel;
	preset.DecayTime = decayTime;
	XAUDIO2FX_REVERB_PARAMETERS Effectinfo;
	ReverbConvertI3DL2ToNative(&preset, &Effectinfo);

	HRESULT hr = sv->SetEffectParameters(slot, &Effectinfo, sizeof(Effectinfo));*/

	FXREVERB_PARAMETERS param;
	param.Diffusion = diffusion;
	param.RoomSize = roomSize;
	HRESULT hr = sv->SetEffectParameters(slot, &param, sizeof(param));
	//isAlter = false;
}

void Reverb::Gui()
{
	if (ImGui::TreeNode("Reverb"))
	{
		ImGui::SliderFloat("diffusion", &diffusion, 0.0f, 1.0f);
		ImGui::SliderFloat("roomSize", &roomSize, 0.0001f, 1.0f);

		ImGui::TreePop();
	}
	ImGui::Separator();
}

Echo::Echo(int slot) :SoundEffect(slot)
{
	CreateFX(__uuidof(FXEcho), &pXAPO);
}

Echo::~Echo()
{
}

void Echo::update(IXAudio2SubmixVoice* sv)
{
	//値が変更されてないなら即リターン
	//if (!isAlter || !sv)return;

	FXECHO_PARAMETERS param;

	param.Delay = delay;
	param.Feedback = feedback;
	param.WetDryMix = wetDryMix;

	HRESULT hr = sv->SetEffectParameters(slot, &param, sizeof(param));
	//isAlter = false;
}

void Echo::Gui()
{
	if (ImGui::TreeNode("Echo"))
	{
		ImGui::SliderFloat("WetDryMix", &wetDryMix, 0.0f, 1.0f);
		ImGui::SliderFloat("feedback", &feedback, 0.0f, 1.0f);
		ImGui::SliderFloat("delay", &delay, 1.0f, 2000.0f);

		ImGui::TreePop();
	}
	ImGui::Separator();
}

Equalizer::Equalizer(int slot) :SoundEffect(slot)
{
	CreateFX(__uuidof(FXEQ), &pXAPO);

}

Equalizer::~Equalizer()
{

}

void Equalizer::update(IXAudio2SubmixVoice* sv)
{
	FXEQ_PARAMETERS eqParams;
	eqParams.FrequencyCenter0 = frequencyCenter.x;
	eqParams.FrequencyCenter1 = frequencyCenter.y;
	eqParams.FrequencyCenter2 = frequencyCenter.z;
	eqParams.FrequencyCenter3 = frequencyCenter.w;

	eqParams.Bandwidth0 = bandwidth.x;
	eqParams.Bandwidth1 = bandwidth.y;
	eqParams.Bandwidth2 = bandwidth.z;
	eqParams.Bandwidth3 = bandwidth.w;

	eqParams.Gain0 = gain.x;
	eqParams.Gain1 = gain.y;
	eqParams.Gain2 = gain.z;
	eqParams.Gain3 = gain.w;

	sv->SetEffectParameters(slot, &eqParams, sizeof(eqParams));
}

void Equalizer::Gui()
{
	if (ImGui::TreeNode("Equalizer"))
	{
		ImGui::SliderFloat("FrequencyCenter0", &frequencyCenter.x, 80.0f, 160.0f);
		ImGui::SliderFloat("FrequencyCenter1", &frequencyCenter.y, 500.0f, 1500.0f);
		ImGui::SliderFloat("FrequencyCenter2", &frequencyCenter.z, 2000.0f, 5000.0f);
		ImGui::SliderFloat("FrequencyCenter3", &frequencyCenter.w, 6000.0f, 120000.0f);

		ImGui::SliderFloat4("Bandwidth", &bandwidth.x, 0.5f, 2.0f);
		ImGui::SliderFloat4("Gain", &gain.x, -6.0f, 6.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();
}