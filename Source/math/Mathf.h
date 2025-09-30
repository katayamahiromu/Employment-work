#pragma once
#include<DirectXMath.h>
#include<math.h>
#include<cmath>

//�g�������l�̒�`
#define TWO_THIRDS (2.0f / 3.0f)
#define ONE_THIRDS (1.0f / 3.0f)

//���w�n�̊֐��B
class Mathf
{
public:
	//���`�⊮
	static float Leap(float a, float b, float t);
	static DirectX::XMFLOAT3 Leap(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, float t);

	//����
	static float Proportion(float a, float b);

	//�w��͈͂̃����_���l���v�Z
	static float RandomRange(float min, float max);

	//�O�����̎擾
	static DirectX::XMFLOAT3 GetForwardDirection(float angleY);

	//Xyz�ł̋����̓����v�Z
	static float calcDistanceSqXYZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	//Xyz�ł̋����̓����v�Z
	static float calcDistanceSqXZ(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	//�p���s����ʒu�A�g��A��]�ɕ���
	static void transformDecomposition(DirectX::XMFLOAT4X4&transform, DirectX::XMFLOAT3&pos, DirectX::XMFLOAT3&scale,DirectX::XMFLOAT4&rotation);

	//�p�x���K��
	static float normalizeAngle(float angle);

	//�N�H�[�^�j�I�����烈�[�p�̒l���擾
	static float getYawFromQuaternion(const DirectX::XMFLOAT4& q);
};