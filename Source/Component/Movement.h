#pragma once
#include"Component.h"
#include"math/Collision.h"
#include<functional>

//�ړ�
class Movement : public Component
{
public:
	Movement();
	virtual ~Movement(){}
	//���O�擾
	const char* getName() const override { return "Movement"; }

	void update(float elapsedTime)override;

	//GUI�`��
	void OnGUI()override;

	// �ړ�
	void move(const DirectX::XMFLOAT3& direction, float elapsedTime);
	void moveLocal(const DirectX::XMFLOAT3& direction, float elapsedTime);

	// ����
	void turn(const DirectX::XMFLOAT3& direction, float elapsedTime);

	void setMoveSpeed(float value) { moveSpeed = value; }
	void setMaxMoveSpeed(float value) { maxMoveSpeed = value; }

	DirectX::XMFLOAT3 getVelocity() { return velocity; }
private:
	//�����ړ��X�V����
	void updateVerticalVelocity(float elapsedTime);

	//�����ړ�����
	void updateVerticalMove(float elapsedTime);

	//�����ړ��X�V����
	void updateHorizontalVelocity(float elapsedTime);

	//�����ړ�����
	void updateHorizontalMove(float elapsedTime);
private:
	float moveSpeed = 5.0f;
	float turnSpeed = 6.28f;

	float gravity = -1.0f;
	//���C
	float friction = 0.5f;
	//�����x
	float acceleration = 1.0f;
	//�ő�ړ����x
	float maxMoveSpeed = 5.0f;
	// �ړ�����
	DirectX::XMFLOAT3 direction = { 0,0,0 };   

	//���x
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	std::function<bool(DirectX::XMFLOAT3,DirectX::XMFLOAT3,HitResult&)>raycast;
};