#include"Audio3DSystem.h"
#include"AudioManager.h"
#include"../math/Mathf.h"
#include"../imgui/imgui.h"
#include<algorithm>


Audio3D::Audio3D(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, SoundEmitter* emitter):Audio(xaudio,resource)
{
	if (emitter != nullptr)
	{
		this->emitter = emitter; // 値のみ代入

		dspSetting.srcChannelCount = resource->getWaveFormat().nChannels;
		dspSetting.dstChannelCount = 2;
		dspSetting.outputMatrix = new float[dspSetting.srcChannelCount * dspSetting.dstChannelCount];
	}
}

Audio3D::~Audio3D()
{
	//if (emitter)delete emitter;
	if (dspSetting.outputMatrix) delete[] dspSetting.outputMatrix;
}

void Audio3D::update(float volume)
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

	setVolume(volume);
	if (emitter != nullptr)
	{
		setPan();
		filter(LowPassOnePoleFilter);
	}
}

void Audio3D::setPan()
{
	XAUDIO2_VOICE_DETAILS voiceDetails;
	sourceVoice->GetVoiceDetails(&voiceDetails);

	XAUDIO2_VOICE_DETAILS masterDetails;
	IXAudio2MasteringVoice* masteringVoice = AudioManager::instance()->getMasteringVoice();
	masteringVoice->GetVoiceDetails(&masterDetails);

	//リスナーと音源の位置関係から出力先配列を適用
	sourceVoice->SetOutputMatrix(masteringVoice, voiceDetails.InputChannels, 2, dspSetting.outputMatrix);
}

void Audio3D::filter(XAUDIO2_FILTER_TYPE type, FLOAT32 overq)
{
	XAUDIO2_FILTER_PARAMETERS filter;
	filter.Type = type;	//使うフィルターの種類
	filter.Frequency = 2.0f * sinf(X3DAUDIO_PI / 6.0f * (1.0f - dspSetting.filterParam));// リスナーと音源の位置関係からとったフィルター係数を適用
	filter.OneOverQ = overq; //実際にどのくらいの音量がカットされているか

	sourceVoice->SetFilterParameters(&filter);
}


float Audio3D::calcAngle(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 vec)
{
	DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b)));
	DirectX::XMVECTOR Vec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vec));

	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Dir, Vec));
	dot = std::clamp(dot, -1.0f, 1.0f);

	return acosf(dot);//ラジアン角
}

void Audio3D::setDSPSetting(SoundListner& listner)
{
	//距離
	DirectX::XMVECTOR Lp = DirectX::XMLoadFloat3(&listner.position);
	DirectX::XMVECTOR Ep = DirectX::XMLoadFloat3(&emitter->position);

	dspSetting.distanceListerEmitter = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(Lp,Ep)));

	//ドップラー効果
	DirectX::XMVECTOR emitterToListener = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&listner.position), DirectX::XMLoadFloat3(&emitter->position));
	emitterToListener = DirectX::XMVector3Normalize(emitterToListener);
	float listenerSpeed = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&listner.velocity), emitterToListener));
	float emitterSpeed = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&emitter->velocity), emitterToListener));

	float soundSpeed = 340.0f;
	dspSetting.dopplerScale = (soundSpeed - listenerSpeed) / (soundSpeed - emitterSpeed);

	//角度
	float angle = calcAngle(emitter->position, listner.position, listner.rightVec);
	dspSetting.radianListerEmitter = angle < M_PI * 0.5f ?
		calcAngle(emitter->position, listner.position, listner.frontVec) :
		-calcAngle(emitter->position, listner.position, listner.frontVec);

	//音の減衰率
	float scaler = max(0.0f, min(1.0f, 1.0f - dspSetting.distanceListerEmitter / emitter->maxDistance));
	this->scale = scaler;
	switch (dspSetting.srcChannelCount * dspSetting.dstChannelCount)
	{
	case 1:
		dspSetting.outputMatrix[0] = scaler;
		break;
	case 4:
		float ang = angle < M_PI * 0.5f ?
			dspSetting.radianListerEmitter : -calcAngle(emitter->position, listner.position, listner.frontVec);
		ang = (dspSetting.radianListerEmitter + 90) * 0.5f;
		float L = cosf(ang);
		float R = sinf(ang);
		if (dspSetting.distanceListerEmitter > emitter->minDistance)
		{
			//音量を触ると重いためパンに減衰率を掛けて最適化
			L *= scaler;
			R *= scaler;
		}
		dspSetting.outputMatrix[0] = dspSetting.outputMatrix[1] = L;
		dspSetting.outputMatrix[2] = dspSetting.outputMatrix[3] = R;

		left = L;
		right = R;
		break;
	}

	//リスナーと音源の角度からローパスに適用する値を計算

	//正面からはずれているか
	float a = std::abs(dspSetting.radianListerEmitter);
	if (a > listner.innerRadius)
	{
		//角度に応じてスケール
		float t = (a - listner.innerRadius) / (listner.outerRadius - listner.innerRadius);

		//１を上限とする（外れ過ぎても最大値止まり）
		t = min(1.0f, t);

		//スケールに応じてフィルターパラメーターを調整
		dspSetting.filterParam = listner.filterParam * t;
	}
	else
	{
		//正面なんでフィルター無し
		dspSetting.filterParam = 0.0f;
	}
}

void Audio3D::gui()
{
	//ImGui::Begin("debug Sound");
	ImGui::InputFloat("distance", &dspSetting.distanceListerEmitter);
	ImGui::InputFloat("Scale", &scale);
	ImGui::InputFloat("Left Pan", &left);
	ImGui::InputFloat("Right Pan", &right);
	//ImGui::End();
}