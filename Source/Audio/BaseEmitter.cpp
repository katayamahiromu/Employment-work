#include"BaseEmitter.h"
#include"math/Mathf.h"
#include<algorithm>

DSPResult& BaseEmitter::calcDSP(const SoundEmitter& emitter, const SoundListner& listener)
{
    DSPResult result{};

    //距離
    result.distance = Mathf::calcDistanceXYZ(emitter.position, listener.position);

    //ドップラー
    result.dopplerScale = Mathf::directionalSpeedRatio(
        listener.position,
        listener.velocity,
        emitter.position,
        emitter.velocity,
        SOUND_SPEED
    );

    //角度
    result.radian = Mathf::signedAngle(
        listener.position,
        listener.frontVec,
        listener.rightVec,
        emitter.position
    );

    //距離減衰率
    result.scale = std::clamp(1.0f - result.distance / emitter.maxDistance, 0.0f, 1.0f);

    //各音源事のフィルター適用のパラメータ
    result.filterParam = calcFilterParam(emitter, listener,result);

    return result;
}

DirectX::XMFLOAT3 BaseEmitter::calcVelocity(const SoundEmitter& emitter)
{
    DirectX::XMFLOAT3 vel;
    DirectX::XMStoreFloat3(&vel,
        DirectX::XMVectorSubtract(
            DirectX::XMLoadFloat3(&emitter.position),
            DirectX::XMLoadFloat3(&prevPosition))
    );

    return vel;
}

float  BaseEmitter::calcFrequencyLPF(float filterPram)
{
    //指数の強さ
    const float power = 2.0f;

    // 指数カーブに変換（0〜1）
    float curved = 1.0f - expf(-power * filterPram);
    return 2.0f * sinf(DirectX::XM_PI / 6.0f * (1.0f - curved));
}