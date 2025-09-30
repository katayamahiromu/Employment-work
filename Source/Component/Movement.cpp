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

//���[���h�ړ�
void Movement::move(const DirectX::XMFLOAT3& direction, float elapsedTime)
{
	this->direction = direction;
	//�������x�X�V����
	updateVerticalVelocity(elapsedTime);

	//�����ړ��X�V����
	updateVerticalMove(elapsedTime);

	//�������x�X�V����
	updateHorizontalVelocity(elapsedTime);

	//�����ړ��X�V����
	updateHorizontalMove(elapsedTime);
}

// ���[�J���ړ�
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

// ����
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


//�����ړ�
void Movement::updateVerticalVelocity(float elapsedTime)
{
	//�d�͏���
	velocity.y += gravity * elapsedTime * 60.0f;
}

//�����ړ�����
void Movement::updateVerticalMove(float elapsedTime)
{
	//���������̈ړ���
	float moveY = velocity.y * elapsedTime;
	DirectX::XMFLOAT3 pos = *getObject()->getPosition();
	if (moveY < 0.0f)
	{
		//���C�̊J�n�ʒu��ݒ�(�����Ƃ�菭����)
		DirectX::XMFLOAT3 start = { pos.x, pos.y + 1.0f, pos.z };
		//���C�̏I�[�ʒu��ݒ�i�ړ���̈ʒu�j
		DirectX::XMFLOAT3 end = { pos.x, pos.y + moveY, pos.z };

		//���C�L���X�g�ɂ��n�ʔ���
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
			//�󒆂ɕ����Ă���
			pos.y += moveY;
		}
	}
	else if (moveY > 0.0f)
	{
		//�㏸��
		pos.y += moveY;
	}

	getObject()->setPosition(pos);
}

//�������x�X�V����
void Movement::updateHorizontalVelocity(float elapsedTime)
{
	// �������̑��x�x�N�g�����쐬�iY�����̓[���Ɂj
	DirectX::XMVECTOR horizontalVelocity = DirectX::XMVectorSet(velocity.x, 0.0f, velocity.z, 0.0f);

	//�x�N�g���̒������v�Z
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(horizontalVelocity));
	
	if (length > 0)
	{
		float friction = this->friction * elapsedTime * 60.0f;
		if (length > friction)
		{
			//���K�����Ė��C�͂��|���A�����x�N�g�������߂�
			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(horizontalVelocity);
			DirectX::XMVECTOR frictionVec = DirectX::XMVectorScale(direction, friction);

			//������K�p
			horizontalVelocity = DirectX::XMVectorSubtract(horizontalVelocity, frictionVec);

			// ���ʂ� velocity.x �� velocity.z �ɔ��f
			velocity.x = DirectX::XMVectorGetX(horizontalVelocity);
			velocity.z = DirectX::XMVectorGetZ(horizontalVelocity);
		}
		else
		{
			//���C�͈ȉ��͒�~
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	
	//�ő呬�x�ȉ��Ȃ�����������s��
	if (length <= maxMoveSpeed)
	{
		//�����x�N�g�����O�łȂ��Ȃ�����������s��
		DirectX::XMVECTOR Direction = DirectX::XMVectorSet(direction.x, 0.0f, direction.z, 0.0f);

		float directionLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Direction));
		if (directionLength > 0.0f)
		{
			//�����x�i�t���[���P�ʂŌv�Z�j
			float acceleration = this->acceleration * elapsedTime * 60.0f;

			//��������
			//�����x�N�g���ɉ����x�����X�P�[�����O�������x�x�N�g���ɉ��Z
			velocity.x += direction.x * acceleration;
			velocity.z += direction.z * acceleration;


			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

			if (length > maxMoveSpeed)
			{
				//�����x�N�g���ɍő�ړ����x���X�P�[�����O�����l
				velocity.x = direction.x * maxMoveSpeed;
				velocity.z = direction.z * maxMoveSpeed;
			}
		}
	}
}

void Movement::updateHorizontalMove(float elapsedTime)
{
	// �ړ�����
	DirectX::XMFLOAT3 pos = *getObject()->getPosition();
	
	DirectX::XMVECTOR Velocity = DirectX::XMVectorSet(velocity.x, 0.0f, velocity.z, 0.0f);
	float velocityLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Velocity));
	if (velocityLength > 0.0f)
	{
		//�v�Z�p�̈ړ���̑��x
		float moveX = velocity.x * elapsedTime;
		float moveZ = velocity.z * elapsedTime;
		float radius = getObject()->getRange();
		//���C�̎n�_�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { pos.x,pos.y + 1.0f,pos.z };
		DirectX::XMFLOAT3 end = { pos.x + moveX,pos.y + 1.0f,pos.z + moveZ };

		//���C�L���X�g
		HitResult result;
		MessageData::RAYCASTREQUEST r = { start,end,result,false };
		Messenger::instance().sendData(MessageData::RAY_CAST_RESULT, &r);
		if (r.isHit)
		{
			DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&r.result.position);
			DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

			//�ǂ̖@���x�N�g��
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&r.result.normal);

			//���˃x�N�g�����t�x�N�g���ɕϊ�
			vec = DirectX::XMVectorNegate(vec);

			//���˃x�N�g����@���Ŏˉe�i�ړ��ʒu����ǂ܂ł̋����j
			DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
			float length;
			DirectX::XMStoreFloat(&length, lengthVec);

			//�␳�ʒu�ւ̃x�N�g�����v�Z
			DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

			//�ŏI�I�ȕ␳�ʒu�x�N�g�����v�Z
			DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
			DirectX::XMFLOAT3 correctPos;
			DirectX::XMStoreFloat3(&correctPos, correctPosVec);


			//�ǂ̐�ɕǂ��������ꍇ�̏���
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
			//�ǂɓ������Ă��Ȃ��ꍇ�͒ʏ�̈ړ�
			pos.x += moveX;
			pos.z += moveZ;
		}
	}
	getObject()->setPosition(pos);
}