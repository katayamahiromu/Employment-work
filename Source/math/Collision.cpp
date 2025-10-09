#include"Collision.h"
#include"Graphics/SkinnedMesh.h"

	/// <summary>
	/// 球と球の交差判定
	/// </summary>
	/// <param name="positionA">球A</param>
	/// <param name="radiusA">半径A</param>
	/// <param name="positionB">球B</param>
	/// <param name="radiusB">半径B</param>
	/// <param name="outVec">衝突方向のベクトル（出力用）</param>
	/// <returns>true...衝突した</returns>
bool Collision::intersectSphereAndSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
	DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
	////二点間の位置ベクトルを引く
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);

	//単一の長さを計算
	float len;
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMStoreFloat(&len, length);

	//距離判定
	float range = radiusA + radiusB;
	if (len > range)return false;

	//衝突していたら衝突方向のベクトルを設定
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMStoreFloat3(&outVec, vec);

	return true;
}

/// <summary>
/// 円柱と円柱の交差判定
/// </summary>
/// <param name="positionA">位置A</param>
/// <param name="radiusA">半径A</param>
/// <param name="heightA">高さA</param>
/// <param name="positionB">位置B</param>
/// <param name="radiusB">半径B</param>
/// <param name="heightB">高さB</param>
/// <param name="outVec">押し出し位置（出力用）</param>
/// <returns>true...衝突した</returns>

bool Collision::intersectCylinderAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	// Aの足元がBの頭より上ならあたっていない
	if (positionA.y > positionB.y + heightB)return false;
	// Aの頭がBの足元より下ならあたっていない
	if (positionA.y + heightA < positionB.y)return false;

	// XZ平面での範囲チェック
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = sqrtf(lengthSq) - (radiusA + radiusB);
	if (range >= 0.0f) return false;

	// AがBを押し出す
	Vec = DirectX::XMVectorScale(Vec, range);
	DirectX::XMVECTOR vec = DirectX::XMVectorAdd(PositionB, Vec);
	DirectX::XMStoreFloat3(&outVec, vec);

	return true;
}

bool Collision::intersectSphereAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	//A(球)の足元がBの(円柱)の頭より上なら衝突してない
	if (positionA.y - radiusA > positionB.y + heightB)return false;

	//A（球）の頭が B（円柱）の足もとより下なら衝突していない
	if (positionA.y + radiusA < positionB.y)return false;

	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//衝突方向のベクトル
	outVec.x = vx / distXZ;
	outVec.y = 0.0f;
	outVec.z = vz / distXZ;

	return true;
}

	/// <summary>
	/// レイとモデルの交差判定
	/// </summary>
	/// <param name="start">レイの始点</param>
	/// <param name="end">レイの終点</param>
	/// <param name="model">モデル</param>
	/// <param name="result">ヒット結果</param>
	/// <returns>true...衝突した</returns>
bool Collision::intersectRayAndModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const SkinnedMesh* model,
	HitResult& result)
{
	//ワールド空間上でのレイの視点
	DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
	//ワールド空間上でのレイの終点
	DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
	//ワールド空間上でのレイの始点から終点までのベクトル
	DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
	//ワールド空間上でのレイの長さ
	DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
	DirectX::XMStoreFloat(&result.distance, worldRayLength);

	
	//true...衝突した
	bool hit = false;

	for (const SkinnedMesh::mesh&mesh : model->meshes)
	{
		//レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh.default_global_transform);
		DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

		//ローカル空間上でのレイの始点
		DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
		//ローカル空間上でのレイの終点
		DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
		//ローカル空間上でのレイの始点から終点までのベクトル
		DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
		//ローカル空間上でのレイの方向ベクトル
		DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
		//ローカル空間上での長さ
		DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
		float localRayLength;
		DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);


		//頂点データを取得
		const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
		const std::vector<UINT>& indices = mesh.indices;

		//候補となる情報
		int materialIndex = -1;
		DirectX::XMVECTOR hitPosition;
		DirectX::XMVECTOR hitNormal;

		for (const SkinnedMesh::mesh::subset& subset : mesh.subsets)
		{
			for (UINT i = 0;i < subset.index_count; i+=3)
			{
				UINT index = subset.start_index_location + i;

				//三角形の頂点の抽出
				const SkinnedMesh::vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

				DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
				DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
				DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

				//三角形の法線を計算
				DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

				//レイの方向と面の方向をチェック（平面の表裏判定）
				DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
				float dot;
				DirectX::XMStoreFloat(&dot, dotVec);
				if (dot >= 0)continue;

				//レイの始点から交点までの長さを計算
				DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
				float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

				//長さが０以下なら以降の処理は飛ばす
				if (length < 0.0f) continue;

				//レイと平面の交点を計算
				DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

				//交点が三角形の内側にあるかどうか判定

				//交点から点Aまでのベクトル
				DirectX::XMVECTOR paVac = DirectX::XMVectorSubtract(aVec, hitPosVec);
				//交点から点Aまでのベクトルと点A点Bのベクトルの外積
				DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVac, abVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
				float dot1;
				DirectX::XMStoreFloat(&dot1, dotVec1);
				if (dot1 < 0.0f)continue;

				
				//交点から点Bまでのベクトル
				DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
				//交点から点Bまでのベクトルと点B点Cのベクトルの外積
				DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
				float dot2;
				DirectX::XMStoreFloat(&dot2, dotVec2);
				if (dot2 < 0.0f)continue;

				
				//交点から点Cまでのベクトル
				DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
				//交点から点Cまでのベクトルと点C点Aのベクトルの外積
				DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
				float dot3;
				DirectX::XMStoreFloat(&dot3, dotVec3);
				if (dot3 < 0.0f)continue;


				//長さが０以下なら以降の処理は飛ばす
				if (length < 0.0f)continue;

				//交点までの距離が今までに計算した最近距離より大きい時はスキップ
				if (length > localRayLength) continue;

				//最近距離を更新
				localRayLength = length;

				//マテリアル番号を更新
				materialIndex = static_cast<int>(subset.material_unique_id);

				//交点と法線を更新
				hitPosition = hitPosVec;
				hitNormal = normalVec;
			}
		}

		if (materialIndex >= 0)
		{
			//交点座標をローカル空間からワールド空間へ変換
			DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
			//ワールド空間上でのレイの始点から交点までのベクトル
			DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
			//ワールド空間上でのレイの始点から交点までの長さ
			DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
			float distance;
			DirectX::XMStoreFloat(&distance, worldLengthVec);

			//ヒット結果情報保存
			if (result.distance > distance)
			{
				//ヒット時の面を法線をローカル空間からワールド空間へ変換
				DirectX::XMVECTOR worldNormal = DirectX::XMVector3Transform(hitNormal, worldTransformMat);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, worldPositionVec);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
				hit = true;
			}
		}
	}

	return hit;
}

bool Collision::squareVsSquare(
	DirectX::XMFLOAT2 posA,
	DirectX::XMFLOAT2 sizeA,
	DirectX::XMFLOAT2 posB,
	DirectX::XMFLOAT2 sizeB
)
{
	DirectX::XMFLOAT4 squareA;
	DirectX::XMFLOAT4 squareB;

	//left
	squareA.x = posA.x;
	//right
	squareA.y = posA.x + sizeA.x;
	//top
	squareA.z = posA.y;
	//bottom
	squareA.w = posA.y + sizeA.y;


	//left
	squareB.x = posB.x;
	//right
	squareB.y = posB.x + sizeB.x;
	//top
	squareB.z = posB.y;
	//bottom
	squareB.w = posB.y + sizeB.y;

	//判定
	if (squareA.y < squareB.x) return false;
	if (squareA.x > squareB.y) return false;
	if (squareA.w < squareB.z) return false;
	if (squareA.z > squareB.w) return false;

	return true;
}