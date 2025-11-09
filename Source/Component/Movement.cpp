#include"Movement.h"
#include"imgui.h"
#include"system/MessageData.h"
#include"system/Messenger.h"

Movement::Movement()
{
}

void Movement::OnGUI()
{
	ImGui::InputFloat("Move Speed", &moveSpeed);
	ImGui::InputFloat("Turn Speed", &turnSpeed);
	ImGui::InputFloat3("velocity", &velocity.x);
}

void Movement::update(float elapsedTime)
{

}

//ワールド移動
void Movement::move(const DirectX::XMFLOAT3& direction, float elapsedTime)
{
	this->direction = direction;
	//垂直速度更新処理
	updateVerticalVelocity(elapsedTime);

	//垂直移動更新処理
	updateVerticalMove(elapsedTime);

	//水平速度更新処理
	updateHorizontalVelocity(elapsedTime);

	//水平移動更新処理
	updateHorizontalMove(elapsedTime);
}

// ローカル移動
void Movement::moveLocal(const DirectX::XMFLOAT3& direction, float elapsedTime)
{
	std::shared_ptr<Object> obj = getObject();
	float speed = moveSpeed * elapsedTime;
	DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
	DirectX::XMVECTOR Velocity = DirectX::XMVectorScale(Direction, speed);
	DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(obj->getRotation());
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationQuaternion(Rotation);
	DirectX::XMVECTOR Move = DirectX::XMVector3TransformNormal(Velocity, Transform);
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(obj->getPosition());

	Position = DirectX::XMVectorAdd(Position, Move);

	DirectX::XMFLOAT3 position;
	DirectX::XMStoreFloat3(&position, Position);
	obj->setPosition(position);
}

// 旋回
void Movement::turn(const DirectX::XMFLOAT3& direction, float elapsedTime)
{
	std::shared_ptr<Object> obj = getObject();
	float speed = turnSpeed * elapsedTime;
	DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
	DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(obj->getRotation());
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationQuaternion(Rotation);
	DirectX::XMVECTOR OneZ = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR Front = DirectX::XMVector3TransformNormal(OneZ, Transform);

	Direction = DirectX::XMVector3Normalize(Direction);
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, Direction);
	if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
	{
		return;
	}

	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Front, Direction);

	float dot;
	DirectX::XMStoreFloat(&dot, Dot);
	speed = (std::min)(1.0f - dot, speed);

	DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
	Rotation = DirectX::XMQuaternionMultiply(Rotation, Turn);

	DirectX::XMFLOAT4 rotation;
	DirectX::XMStoreFloat4(&rotation, Rotation);
	obj->setRotation(rotation);
}


//垂直移動
void Movement::updateVerticalVelocity(float elapsedTime)
{
	//重力処理
	velocity.y += gravity * elapsedTime * 60.0f;
}

//垂直移動処理
void Movement::updateVerticalMove(float elapsedTime)
{
	//垂直方向の移動量
	float moveY = velocity.y * elapsedTime;
	DirectX::XMFLOAT3 pos = *getObject()->getPosition();
	if (moveY < 0.0f)
	{
		//レイの開始位置を設定(足もとより少し上)
		DirectX::XMFLOAT3 start = { pos.x, pos.y + 1.0f, pos.z };
		//レイの終端位置を設定（移動後の位置）
		DirectX::XMFLOAT3 end = { pos.x, pos.y + moveY, pos.z };

		//レイキャストによる地面判定
		HitResult result;
		MessageData::RAYCASTREQUEST r = { start,end,result,false };
		Messenger::instance().sendData(MessageData::RAY_CAST_RESULT, &r);
		if (r.isHit)
		{
			pos.y = r.result.position.y;
			velocity.y = 0.0f;
		}
		else
		{
			//空中に浮いている
			pos.y += moveY;
		}
	}
	else if (moveY > 0.0f)
	{
		//上昇中
		pos.y += moveY;
	}

	getObject()->setPosition(pos);
}

//垂直速度更新処理
void Movement::updateHorizontalVelocity(float elapsedTime)
{
	// 横方向の速度ベクトルを作成（Y成分はゼロに）
	DirectX::XMVECTOR horizontalVelocity = DirectX::XMVectorSet(velocity.x, 0.0f, velocity.z, 0.0f);

	//ベクトルの長さを計算
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(horizontalVelocity));
	
	if (length > 0)
	{
		float friction = this->friction * elapsedTime * 60.0f;
		if (length > friction)
		{
			//正規化して摩擦力を掛け、減速ベクトルを求める
			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(horizontalVelocity);
			DirectX::XMVECTOR frictionVec = DirectX::XMVectorScale(direction, friction);

			//減速を適用
			horizontalVelocity = DirectX::XMVectorSubtract(horizontalVelocity, frictionVec);

			// 結果を velocity.x と velocity.z に反映
			velocity.x = DirectX::XMVectorGetX(horizontalVelocity);
			velocity.z = DirectX::XMVectorGetZ(horizontalVelocity);
		}
		else
		{
			//摩擦力以下は停止
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	
	//最大速度以下なら加速処理を行う
	if (length <= maxMoveSpeed)
	{
		//方向ベクトルが０でないなら加速処理を行う
		DirectX::XMVECTOR Direction = DirectX::XMVectorSet(direction.x, 0.0f, direction.z, 0.0f);

		float directionLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Direction));
		if (directionLength > 0.0f)
		{
			//加速度（フレーム単位で計算）
			float acceleration = this->acceleration * elapsedTime * 60.0f;

			//加速処理
			//方向ベクトルに加速度えをスケーリングした速度ベクトルに加算
			velocity.x += direction.x * acceleration;
			velocity.z += direction.z * acceleration;


			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

			if (length > maxMoveSpeed)
			{
				//方向ベクトルに最大移動速度をスケーリングした値
				velocity.x = direction.x * maxMoveSpeed;
				velocity.z = direction.z * maxMoveSpeed;
			}
		}
	}
}

void Movement::updateHorizontalMove(float elapsedTime)
{
	// 移動処理
	DirectX::XMFLOAT3 pos = *getObject()->getPosition();
	
	DirectX::XMVECTOR Velocity = DirectX::XMVectorSet(velocity.x, 0.0f, velocity.z, 0.0f);
	float velocityLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Velocity));
	if (velocityLength > 0.0f)
	{
		//計算用の移動後の速度
		float moveX = velocity.x * elapsedTime;
		float moveZ = velocity.z * elapsedTime;
		float radius = getObject()->getRange();
		//レイの始点位置と終点位置
		DirectX::XMFLOAT3 start = { pos.x,pos.y + 1.0f,pos.z };
		DirectX::XMFLOAT3 end = { pos.x + moveX,pos.y + 1.0f,pos.z + moveZ };

		//レイキャスト
		HitResult result;
		MessageData::RAYCASTREQUEST r = { start,end,result,false };
		Messenger::instance().sendData(MessageData::RAY_CAST_RESULT, &r);
		if (r.isHit)
		{
			DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&r.result.position);
			DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

			//壁の法線ベクトル
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&r.result.normal);

			//入射ベクトルを逆ベクトルに変換
			vec = DirectX::XMVectorNegate(vec);

			//入射ベクトルを法線で射影（移動位置から壁までの距離）
			DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
			float length;
			DirectX::XMStoreFloat(&length, lengthVec);

			//補正位置へのベクトルを計算
			DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

			//最終的な補正位置ベクトルを計算
			DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
			DirectX::XMFLOAT3 correctPos;
			DirectX::XMStoreFloat3(&correctPos, correctPosVec);


			//壁の先に壁が合った場合の処理
			start = r.result.position;
			end = correctPos;
			HitResult result2;
			MessageData::RAYCASTREQUEST r2 = { start,end,result2,false };
			Messenger::instance().sendData(MessageData::RAY_CAST_RESULT, &r2);
			if (r2.isHit)
			{
				pos.x = r2.result.position.x;
				pos.z = r2.result.position.z;
			}
			else
			{
				pos.x = correctPos.x;
				pos.z = correctPos.z;
			}
		}
		else
		{
			//壁に当たっていない場合は通常の移動
			pos.x += moveX;
			pos.z += moveZ;
		}
	}
	getObject()->setPosition(pos);
}