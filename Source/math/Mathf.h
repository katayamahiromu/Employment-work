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
	static inline float Proportion(float a, float b);
	static inline float relativeRate(float base, float observer, float source);
	//指定範囲のランダム値を計算
	static float RandomRange(float min, float max);

	//前方向の取得
	static DirectX::XMFLOAT3 GetForwardDirection(float angleY);

	//正規化した方向ベクトルの取得
	static DirectX::XMVECTOR calcDir(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);

	//Xyzでの距離の二乗を計算
	static float calcDistanceSqXYZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);
	static float calcDistanceXYZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);
	//Xyzでの距離の二乗を計算
	static float calcDistanceSqXZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	//姿勢行列を位置、拡大、回転に分解
	static void transformDecomposition(
		DirectX::XMFLOAT4X4&transform, 
		DirectX::XMFLOAT3&pos, 
		DirectX::XMFLOAT3&scale,
		DirectX::XMFLOAT4&rotation);

	//角度正規化
	static float normalizeAngle(float angle);

	//クォータニオンからヨー角の値を取得
	static float getYawFromQuaternion(const DirectX::XMFLOAT4& q);

	//線分と点を比べ一番近い点を算出
	static DirectX::XMFLOAT3 ClosestPointOnSegment(
		const DirectX::XMFLOAT3& p0,
		const DirectX::XMFLOAT3& p1,
		const DirectX::XMFLOAT3& point);

	//スプライン上の点を求める
	static DirectX::XMFLOAT3 CatmullRom(
		const DirectX::XMFLOAT3& p0,
		const DirectX::XMFLOAT3& p1,
		const DirectX::XMFLOAT3& p2,
		const DirectX::XMFLOAT3& p3,
		float t);


	//２点が作る方向と任意ベクトルの角度を求める
	static float calcAngle(
		const DirectX::XMFLOAT3& a,
		const DirectX::XMFLOAT3& b,
		const DirectX::XMFLOAT3& vec);

	//スカラー射影　ある方向にどれだけ進んでいるか求める
	static inline float scalarProjection(
		DirectX::XMFLOAT3 v, 
		DirectX::XMFLOAT3 unitDir);

	static inline float scalarProjection(
		DirectX::XMFLOAT3 v, 
		DirectX::XMVECTOR unitDir);

	static inline float scalarProjection(
		DirectX::XMVECTOR  v,
		DirectX::XMVECTOR unitDir);

	//相対的な速度を求める
	static float directionalSpeedRatio(
		const DirectX::XMFLOAT3& observerPos,
		const DirectX::XMFLOAT3& observerVel,
		const DirectX::XMFLOAT3& sourcePos,
		const DirectX::XMFLOAT3& sourceVel,
		float baseSpeed);

	//符号付きの角度を返す
	static float signedAngle(
		const DirectX::XMFLOAT3& observerPos,
		const DirectX::XMFLOAT3& observerForward,
		const DirectX::XMFLOAT3& observerRight,
		const DirectX::XMFLOAT3& sourcePos);
};