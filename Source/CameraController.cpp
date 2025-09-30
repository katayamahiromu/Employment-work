#include"CameraController.h"
#include"Camera.h"
#include"Input/InputManager.h"
#include"math/Mathf.h"
#include"imgui.h"
#include"system/MessageData.h"
#include"system/Messenger.h"
#include<algorithm>

CameraController::CameraController()
{
	position = *Camera::instance()->getEye();
	newPosition = *Camera::instance()->getEye();

	CAMERACHANGEFREEMODEKEY = Messenger::instance().addReceiver(MessageData::CAMERACHANGEFREEMODE, [&](void* data) { onFreeMode(data); });
}

CameraController::~CameraController()
{
	Messenger::instance().removeReceiver(CAMERACHANGEFREEMODEKEY);
}

//�X�V����
void CameraController::update(float elapsedTime)
{
	//�J�����ʒu
	computeEye(elapsedTime);

	//�J�����̉�]
	rotateCameraAngle(elapsedTime);

	//�n�`�Ƃ̓����蔻��
	HitResult result;
	MessageData::RAYCASTREQUEST r = { newTarget,newPosition,result,false };
	Messenger::instance().sendData(MessageData::RAY_CAST_RESULT, &r);
	if (r.isHit)
	{
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&r.result.position);
		DirectX::XMVECTOR cuv = DirectX::XMVectorSet(0, 1, 0, 0);
		p = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorReplicate(4), cuv, p);
		DirectX::XMStoreFloat3(&newPosition, p);
	}

	//�����_����������Ĉړ�����悤�ɂ���
	static	constexpr	float	Speed = 1.0f / 8.0f;
	position.x += (newPosition.x - position.x) * Speed;
	position.y += (newPosition.y - position.y) * Speed;
	position.z += (newPosition.z - position.z) * Speed;
	target.x += (newTarget.x - target.x) * Speed;
	target.y += (newTarget.y - target.y) * Speed;
	target.z += (newTarget.z - target.z) * Speed;

	//�J�����̎��_�ƒ����_��ݒ�
	Camera::instance()->setLookAt(position, target, { 0,1,0 });
}

void CameraController::debugUpdate(float elapsedTime)
{
	Mouse* mouse = InputManager::instance()->getMouse();

	float moveX = (mouse->getPositionX() - mouse->getPrevPositionX()) * 0.02f;
	float moveY = (mouse->getPositionY() - mouse->getPrevPositionY()) * 0.02f;

	Camera* camera = Camera::instance();
	// �����s��𐶐�
	DirectX::XMMATRIX V;
	{
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		// �}�E�X����
		{
			if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				// Y����]
				rotateY += moveX * 0.5f;
				if (rotateY > DirectX::XM_PI)
					rotateY -= DirectX::XM_2PI;
				else if (rotateY < -DirectX::XM_PI)
					rotateY += DirectX::XM_2PI;
				// X����]
				rotateX += moveY * 0.5f;
				if (rotateX > DirectX::XMConvertToRadians(89.9f))
					rotateX = DirectX::XMConvertToRadians(89.9f);
				else if (rotateX < -DirectX::XMConvertToRadians(89.9f))
					rotateX = -DirectX::XMConvertToRadians(89.9f);
			}
			else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
			{
				V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&camera_position),
					DirectX::XMLoadFloat3(&camera_focus),
					up);
				DirectX::XMFLOAT4X4 W;
				DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
				// ���s�ړ�
				float s = distance * 0.035f;
				float x = moveX * s;
				float y = moveY * s;
				camera_focus.x -= W._11 * x;
				camera_focus.y -= W._12 * x;
				camera_focus.z -= W._13 * x;

				camera_focus.x += W._21 * y;
				camera_focus.y += W._22 * y;
				camera_focus.z += W._23 * y;
			}
			int i = mouse->getWheel();
			if (i != 0)	// �Y�[��
			{
				distance -= static_cast<float>(mouse->getWheel()) * distance * 0.001f;
			}
		}
		float sx = ::sinf(rotateX), cx = ::cosf(rotateX);
		float sy = ::sinf(rotateY), cy = ::cosf(rotateY);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera_focus);
		DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
		Front = DirectX::XMVectorMultiply(Front, Distance);
		DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
		DirectX::XMStoreFloat3(&camera_position, Eye);
		// �J�����Ɏ��_�𒍎��_��ݒ�
		Camera::instance()->setLookAt(camera_position, camera_focus, { 0, 1, 0 });
	}

	if (mouse->getWheel() != 0)
	{
		// �Y�[��
		distance -= static_cast<float>(mouse->getWheel()) * distance * 0.001f;
	}
}

void CameraController::OnGui()
{
	ImGui::Begin("camera parameter");
	ImGui::SliderFloat("range", &range,1.0f,30.0f);
	ImGui::SliderFloat("spring", &spring,10.0f,30.0f);
	ImGui::SliderFloat("damping", &damping,1.0f,5.0f);

	DirectX::XMFLOAT3 eye = *Camera::instance()->getEye();
	ImGui::InputFloat3("eye", &eye.x);
	ImGui::InputFloat3("Target", &target.x);
	ImGui::InputFloat3("angle", &angle.x);
	ImGui::InputFloat3("position", &position.x);
	ImGui::End();
}

void CameraController::rotateCameraAngle(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float arx = gamePad->getAxisRX(); //J�L�[��L�L�[
	float ary = gamePad->getAxisRY(); //I�L�[��K�L�[

	//�J�����̉�]���x
	constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
	float speed = rollSpeed * elapsedTime;

	//�X�e�B�b�N�ɍ��킹�ĉ�]
	angle.x += ary * speed; //������]
	angle.y += arx * speed; //Y����]

	constexpr float maxAngle = DirectX::XMConvertToRadians(45);
	constexpr float minAngle = DirectX::XMConvertToRadians(-45);

	//x���̃J������]����
	if (angle.x < minAngle) angle.x = minAngle;
	if (angle.x > maxAngle) angle.x = maxAngle;

	//Y ���̉�]�l�� -3.14 �` 3.14 �Ɏ��܂�悤����
	if (angle.y > DirectX::XM_PI)angle.y -= DirectX::XM_2PI;
	else if (angle.y < -DirectX::XM_PI)angle.y += DirectX::XM_2PI;

	//������]
	float alx = gamePad->getAxisLX();

	if (fabsf(arx) > 0.1f || fabsf(ary) > 0.1f)
	{
		targetAngleY = frontY;
	}
	if (fabsf(alx) < 0.1f)
	{
		targetAngleY = angle.y;
	}

	//newPosition = position;

	/*if ((aly) > 0.1f)
	{
		angle.x *= 0.975f;
	}*/

	angle.y += Mathf::normalizeAngle(targetAngleY - angle.y) / 128.0f;
	angle.y = Mathf::normalizeAngle(angle.y);

	//float alx = gamePad->getAxisLX();
	//float aly = gamePad->getAxisLY();
	//bool isPlayerMoving = fabsf(alx) > 0.1f || fabsf(aly) > 0.1f;
	//bool isCameraRotating = fabsf(arx) > 0.1f || fabsf(ary) > 0.1f;

	//if (!isCameraRotating && isPlayerMoving) {
	//	// �v���C���[�������Ă��ăJ�������삵�ĂȂ����͔w��ɒǏ]
	//	float diffY = Mathf::normalizeAngle(frontY - angle.y);
	//	angle.y += diffY * elapsedTime;
	//}
	//angle.y = Mathf::normalizeAngle(angle.y);
}

void CameraController::computeEye(float elapsedTime)
{
	//�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//�J�����̉�]�s�񂩂�O���@�x�N�g�������o��
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	////�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	newPosition.x = target.x - front.x * range;
	newPosition.y = target.y - front.y * range;
	newPosition.z = target.z - front.z * range;
}

void CameraController::computeEyeTPS(float elapsedTime)
{
	//�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//�J�����̉�]�s�񂩂�O���@�x�N�g�������o��
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	//���z�̈ʒu���Z�o
	DirectX::XMVECTOR idealEyeVec = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&target),
		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&front), range));

	//�X�v�����O�@�_���s���O
	DirectX::XMFLOAT3 diff;
	DirectX::XMStoreFloat3(&diff,
		DirectX::XMVectorSubtract(idealEyeVec, DirectX::XMLoadFloat3(Camera::instance()->getEye())));

	DirectX::XMFLOAT3 force;
	DirectX::XMStoreFloat3(&force,
		DirectX::XMVectorSubtract(
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&diff), spring),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&velocity), damping)
		));

	velocity.x += force.x * elapsedTime;
	velocity.y += force.y * elapsedTime;
	velocity.z += force.z * elapsedTime;

	DirectX::XMFLOAT3 eye = *Camera::instance()->getEye();

	newPosition.x += velocity.x * elapsedTime;
	newPosition.y += velocity.y * elapsedTime;
	newPosition.z += velocity.z * elapsedTime;
}

void CameraController::onFreeMode(void* data)
{
	//�f�[�^�̎󂯎��
	MessageData::CAMERACHANGEFREEMODEDATA* p = static_cast<MessageData::CAMERACHANGEFREEMODEDATA*>(data);
	targetAngleY = p->angle.y;
	newTarget = p->target;
	newTarget.y += 0.01f;
}