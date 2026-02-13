#include"Mathf.h"
#include<stdlib.h>
#include<algorithm>

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

DirectX::XMFLOAT3 Mathf::ClosestPointOnSegment(
    const DirectX::XMFLOAT3& p0,
    const DirectX::XMFLOAT3& p1,
    const DirectX::XMFLOAT3& point)
{
    DirectX::XMVECTOR P0 = DirectX::XMLoadFloat3(&p0);
    DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&p1);
    DirectX::XMVECTOR Point = DirectX::XMLoadFloat3(&point);

    DirectX::XMVECTOR LineVec = DirectX::XMVectorSubtract(P1, P0);
    DirectX::XMVECTOR PtoP1Vec = DirectX::XMVectorSubtract(Point, P0);

    //長さを取得
    float pLenSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(LineVec));

    if (pLenSq <= 0.0001f)
    {
        return p0; // 線分が点の場合
    }

    //射影係数
    float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(PtoP1Vec, LineVec)) / pLenSq;

    t = std::clamp(t, 0.0f, 1.0f);
    DirectX::XMVECTOR Closest = DirectX::XMVectorAdd(P0, DirectX::XMVectorScale(LineVec,t));

    DirectX::XMFLOAT3 result;
    XMStoreFloat3(&result, Closest);
    return result;
}

DirectX::XMFLOAT3 Mathf::CatmullRom(
    const DirectX::XMFLOAT3& p0,
    const DirectX::XMFLOAT3& p1,
    const DirectX::XMFLOAT3& p2,
    const DirectX::XMFLOAT3& p3,
    float t)
{
    DirectX::XMVECTOR P0 = DirectX::XMLoadFloat3(&p0);
    DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&p1);
    DirectX::XMVECTOR P2 = DirectX::XMLoadFloat3(&p2);
    DirectX::XMVECTOR P3 = DirectX::XMLoadFloat3(&p3);

    DirectX::XMVECTOR result =
        DirectX::XMVectorCatmullRom(P0, P1, P2, P3, t);

    DirectX::XMFLOAT3 out;
    XMStoreFloat3(&out, result);
    return out;
}