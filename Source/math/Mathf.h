#pragma once
#include<DirectXMath.h>
#include<math.h>
#include<cmath>

//使いたい値の定義
#define TWO_THIRDS (2.0f / 3.0f)
#define ONE_THIRDS (1.0f / 3.0f)

//数学系の関数達
class Mathf
{
public:
	//線形補完
	static float Leap(float a, float b, float t);
	static DirectX::XMFLOAT3 Leap(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, float t);

	//割合
	static float Proportion(float a, float b);

	//指定範囲のランダム値を計算
	static float RandomRange(float min, float max);

	//前方向の取得
	static DirectX::XMFLOAT3 GetForwardDirection(float angleY);

	//Xyzでの距離の二乗を計算
	static float calcDistanceSqXYZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	//Xyzでの距離の二乗を計算
	static float calcDistanceSqXZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	//姿勢行列を位置、拡大、回転に分解
	static void transformDecomposition(DirectX::XMFLOAT4X4&transform, DirectX::XMFLOAT3&pos, DirectX::XMFLOAT3&scale,DirectX::XMFLOAT4&rotation);

	//角度正規化
	static float normalizeAngle(float angle);

	//クォータニオンからヨー角の値を取得
	static float getYawFromQuaternion(const DirectX::XMFLOAT4& q);
};