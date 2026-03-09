#include"Audio3DSystem.h"
#include"AudioManager.h"
#include"../math/Mathf.h"
#include"../imgui/imgui.h"
#include<algorithm>

Audio3D::Audio3D(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, std::shared_ptr<BaseEmitter>emitterType, SoundEmitter* emitter):Audio(xaudio,resource,false)
{
    //各種Emiiterの設定
	if (emitter != nullptr)
	{
		this->emitter = emitter; // 値のみ代入

		dspSetting.srcChannelCount = resource->getWaveFormat().nChannels;

		IXAudio2MasteringVoice* masteringVoice = AudioManager::instance()->getMasteringVoice();

		XAUDIO2_VOICE_DETAILS details;
		masteringVoice->GetVoiceDetails(&details);

		dspSetting.dstChannelCount = details.InputChannels;
		dspSetting.outputMatrix = new float[dspSetting.srcChannelCount * dspSetting.dstChannelCount];
	}

    this->emitterType = emitterType;

    //エフェクトの設定
    effect = AudioManager::instance()->createSubMixVoice();
    effect->addEffect(std::make_unique<Reverb>(0));
    effect->addEffect(std::make_unique<Echo>(1));
    effect->applyEffect();
}

Audio3D::~Audio3D()
{
	if (dspSetting.outputMatrix) delete[] dspSetting.outputMatrix;
}

void Audio3D::update3D(SoundListner& listener)
{
    //エミターの位置と速度
    emitterType->calcEmitterSetting(*emitter);

    //DSPに用いる値計算
	DSPResult result = emitterType->calcDSP(*emitter, listener);

    //DSPの値を用いた各種効果の適応
    calcPan(result);
	filter(LowPassOnePoleFilter, result.filterParam, 1.0f);
	sourceVoice->SetFrequencyRatio(result.dopplerScale);

    //リバーブの値更新
    updateReverb(result);
    updateEcho(result);
    effect->update();
}

void Audio3D::filter(XAUDIO2_FILTER_TYPE type, float filterParam, FLOAT32 overq)
{
	XAUDIO2_FILTER_PARAMETERS filter;
	filter.Type = type;	//使うフィルターの種類
    filter.Frequency = emitterType->calcFrequencyLPF(filterParam);// リスナーと音源の位置関係からとったフィルター係数を適用
	filter.OneOverQ = overq; //実際にどのくらいの音量がカットされているか

	sourceVoice->SetFilterParameters(&filter);
}

void Audio3D::calcPan(DSPResult& result)
{
    const int srcCh = dspSetting.srcChannelCount;
    const int dstCh = dspSetting.dstChannelCount;

    const auto& layout = SpeakerLayout[dstCh];
    float* out = dspSetting.outputMatrix;

    // 出力クリア
    std::fill(out, out + srcCh * dstCh, 0.0f);

    // 角度（0～2π前提）
    float angle = result.radian;
    if (angle < 0.0f) angle += DirectX::XM_2PI;

    // スピーカー探索（LFEスキップ）
    int idxA = -1;
    int idxB = -1;
    float angA = 0.0f;
    float angB = 0.0f;

    const int LFE_INDEX = (dstCh >= 6) ? 3 : -1;

    for (int i = 0; i < dstCh; i++)
    {
        if (i == LFE_INDEX) continue;

        int next = (i + 1) % dstCh;
        if (next == LFE_INDEX) next = (next + 1) % dstCh;

        float a0 = layout[i].azimuth;
        float a1 = layout[next].azimuth;

        // wrap対応
        if (a1 < a0) a1 += DirectX::XM_2PI;

        float a = angle;
        if (a < a0) a += DirectX::XM_2PI;

        if (a >= a0 && a <= a1)
        {
            idxA = i;
            idxB = next;
            angA = a0;
            angB = a1;
            angle = a;
            break;
        }
    }

    if (idxA < 0) return;

    // パン（VBAP近似）
    float t = (angle - angA) / (angB - angA);
    t = std::clamp(t, 0.0f, 1.0f);

    //パン強調
    t = powf(t, panPower);
    float gainA = powf(1.0f - t, panPower);
    float gainB = powf(t, panPower);

    // 疑似等電力
    gainA *= gainA;
    gainB *= gainB;

    // 前後補正
    float front = cosf(angle);
    front = std::clamp(front, 0.0f, 1.0f);

    float frontBoost = 0.6f + 0.4f * front;

    gainA *= frontBoost;
    gainB *= frontBoost;

    // 距離減衰
    gainA *= result.scale;
    gainB *= result.scale;

    // 出力
    for (int s = 0; s < srcCh; s++)
    {
        out[s * dstCh + idxA] = gainA;
        out[s * dstCh + idxB] = gainB;

        // LFE（少し送る）
        if (LFE_INDEX >= 0)
        {
            out[s * dstCh + LFE_INDEX] = result.scale * 0.25f;
        }
    }

    // 適用
    sourceVoice->SetOutputMatrix(
        AudioManager::instance()->getMasteringVoice(),
        srcCh,
        dstCh,
        out
    );
}

void Audio3D::updateReverb(DSPResult& result)
{
    Reverb* reverb = dynamic_cast<Reverb*>(effect->getEffect(REVERB));

    //距離が伸びるほどリバーブを強く
    float normalizeDistance = std::clamp((result.distance - emitter->minDistance) / (emitter->maxDistance - emitter->minDistance), 0.0f, 1.0f);
    const float minWetDryMix = 25.0f;
    const float maxWetDryMix = 80.0f;
    reverb->setWetDryMix(Mathf::Leap(minWetDryMix, maxWetDryMix, normalizeDistance));

    //距離減衰を反映
    float baseGain = Mathf::Leap(-6.0f, -2.0f, normalizeDistance);
    float scaleGain = 10.0f * log10(result.scale);
    reverb->setReverbGain(baseGain + scaleGain);

    //残響時間の変動
    const float minDecayTime = 1.0f;
    const float maxDecayTime = 1.5f;
    reverb->setDecayTime(Mathf::Leap(minDecayTime, maxDecayTime, normalizeDistance));

    float t = std::clamp((result.filterParam - 2000.0f) / (8000.0f - 2000.0f), 0.0f, 1.0f);
    reverb->setRoomFilterHF(Mathf::Leap(-12.0f, -2.0f, t));
}

void Audio3D::updateEcho(DSPResult& result)
{
    Echo* echo = dynamic_cast<Echo*>(effect->getEffect(ECHO));
    float normalizeDistance = std::clamp((result.distance - emitter->minDistance) / (emitter->maxDistance - emitter->minDistance), 0.0f, 1.0f);

    //遠いほど反射音が目立つ
    const float minWetDry = 0.05f;
    const float maxWetDry = 0.80f;
    echo->setWetDryMix(Mathf::Leap(minWetDry, maxWetDry, normalizeDistance));

    //遠い程反射が少し残る
    const float minFeedback = 0.05f;
    const float maxFeedback = 0.30f;
    echo->setFeedback(Mathf::Leap(minFeedback, maxFeedback, normalizeDistance));

    //遠い程反射が遅れる
    const float minDelayMs = 30.0f;
    const float maxDelayMs = 120.0f;
    float delayMs = Mathf::Leap(minDelayMs, maxDelayMs, normalizeDistance);

    // サンプル数に変換
    float sampleRate = 44000.0f;
    float delaySamples = delayMs * 0.001f * sampleRate;

    echo->setDelay(delaySamples);
}

void Audio3D::gui()
{
	//ImGui::Begin("debug Sound");
	/*ImGui::InputFloat("distance", &dspSetting.distanceListerEmitter);
	ImGui::InputFloat("Scale", &scale);
	ImGui::InputFloat("Left Pan", &left);
	ImGui::InputFloat("Right Pan", &right);*/
    for (int i = 0; i < dspSetting.dstChannelCount; i++)
    {
        float gain = dspSetting.outputMatrix[i];
        ImGui::Text("Ch %d : %.3f", i, gain);
    }
    //左右の大きさを強調
    ImGui::SliderFloat("PanPower", &panPower, 1.0, 4.0f);
    effect->Gui();
	//ImGui::End();
}