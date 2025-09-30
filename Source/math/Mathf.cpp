#include"Mathf.h"
#include<stdlib.h>

float Mathf::Leap(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

DirectX::XMFLOAT3 Mathf::Leap(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b,float t)
{
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b), t));
    return result;
}

float Mathf::Proportion(float a, float b)
{
    return a / (a + b);
}


//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
    // 0から1の間のランダムな値を生成
    float random01 = static_cast<float>(rand()) / RAND_MAX;

    // minからmaxの範囲のランダムな値を計算して返す
    return min + (max - min) * random01;
}

//前方向の取得
DirectX::XMFLOAT3 Mathf::GetForwardDirection(float angleY)
{
    return { sinf(angleY),0.0f, cosf(angleY) };
}

float Mathf::calcDistanceSqXYZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b));
    DirectX::XMVECTOR lengthSq = DirectX::XMVector3LengthSq(vec);
    return DirectX::XMVectorGetX(lengthSq);
}

float Mathf::calcDistanceSqXZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    DirectX::XMFLOAT2 Apos = { a.x,a.z };
    DirectX::XMFLOAT2 Bpos = { b.x,b.z };
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat2(&Apos), DirectX::XMLoadFloat2(&Bpos));
    DirectX::XMVECTOR lengthSq = DirectX::XMVector2LengthSq(vec);
    return DirectX::XMVectorGetX(lengthSq);
}

void Mathf::transformDecomposition(DirectX::XMFLOAT4X4& transform, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3&scale, DirectX::XMFLOAT4& rotation)
{
    DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&pos);
    DirectX::XMVECTOR Scale = DirectX::XMLoadFloat3(&scale);
    DirectX::XMVECTOR Rot = DirectX::XMLoadFloat4(&rotation);

    DirectX::XMMatrixDecompose(&Scale, &Rot, &Pos,Transform);

    DirectX::XMStoreFloat3(&pos, Pos);
    DirectX::XMStoreFloat3(&scale, Scale);
    DirectX::XMStoreFloat4(&rotation, Rot);
}

float Mathf::normalizeAngle(float angle)
{
    return atan2f(sinf(angle), cosf(angle));
}

float Mathf::getYawFromQuaternion(const DirectX::XMFLOAT4& q)
{
    DirectX::XMVECTOR quat = XMLoadFloat4(&q);

    // クォータニオンを回転行列に変換
    DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(quat);

    // Yaw を抽出
    // rotMatrix の forwardベクトル（Z軸）から Yaw を推定
    DirectX::XMFLOAT3 forward;
    DirectX::XMStoreFloat3(&forward, rotMatrix.r[2]); // Z軸

    // atan2(forward.x, forward.z) で Yaw を取得
    return std::atan2(forward.x, forward.z);
}