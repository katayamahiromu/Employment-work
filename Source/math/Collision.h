#pragma once
#include<DirectXMath.h>

class SkinnedMesh;

//ヒット結果（レイキャスト後に取得したい情報）
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };//レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0,0,0 }; //衝突したポリゴンの法線ベクトル
	float distance = 0.0f; //レイの視点から交点までの距離
	int materialIndex = -1; //衝突したポリゴンのマテリアル番号
};

class Collision
{
public:
	/// <summary>
	/// 球と球の交差判定
	/// </summary>
	/// <param name="positionA">球A</param>
	/// <param name="radiusA">半径A</param>
	/// <param name="positionB">球B</param>
	/// <param name="radiusB">半径B</param>
	/// <param name="outVec">衝突方向のベクトル（出力用）</param>
	/// <returns>true...衝突した</returns>
	static bool intersectSphereAndSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// 円柱と円柱の交差判定
	/// </summary>
	/// <param name="positionA">位置A</param>
	/// <param name="radiusA">半径A</param>
	/// <param name="heightA">高さA</param>
	/// <param name="positionB">位置B</param>
	/// <param name="radiusB">半径B</param>
	/// <param name="heightB">高さB</param>
	/// <param name="outVec">衝突方向のベクトル（出力用）</param>
	/// <returns>true...衝突した</returns>
	static bool intersectCylinderAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// 球と円柱の交差反映
	/// </summary>
	/// <param name="positionA">球の位置</param>
	/// <param name="radiusA">球の半径</param>
	/// <param name="positionB">円柱の位置</param>
	/// <param name="radiusB">円柱の半径</param>
	/// <param name="heightB">円柱の高さ</param>
	/// <param name="outVec">衝突方向のベクトル（出力用）</param>
	/// <returns>true...衝突した</returns>
	static bool intersectSphereAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// レイとモデルの交差判定
	/// </summary>
	/// <param name="start">レイの始点</param>
	/// <param name="end">レイの終点</param>
	/// <param name="model">モデル</param>
	/// <param name="result">ヒット結果</param>
	/// <returns>true...衝突した</returns>
	static bool intersectRayAndModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const SkinnedMesh* model,
		HitResult& result);

	static bool squareVsSquare(
		DirectX::XMFLOAT2 posA,
		DirectX::XMFLOAT2 sizeA,
		DirectX::XMFLOAT2 posB,
		DirectX::XMFLOAT2 sizeB
	);
};