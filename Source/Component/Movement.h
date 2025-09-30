#pragma once
#include"Component.h"
#include"math/Collision.h"
#include<functional>

//移動
class Movement : public Component
{
public:
	Movement();
	virtual ~Movement(){}
	//名前取得
	const char* getName() const override { return "Movement"; }

	void update(float elapsedTime)override;

	//GUI描画
	void OnGUI()override;

	// 移動
	void move(const DirectX::XMFLOAT3& direction, float elapsedTime);
	void moveLocal(const DirectX::XMFLOAT3& direction, float elapsedTime);

	// 旋回
	void turn(const DirectX::XMFLOAT3& direction, float elapsedTime);

	void setMoveSpeed(float value) { moveSpeed = value; }
	void setMaxMoveSpeed(float value) { maxMoveSpeed = value; }

	DirectX::XMFLOAT3 getVelocity() { return velocity; }
private:
	//垂直移動更新処理
	void updateVerticalVelocity(float elapsedTime);

	//垂直移動処理
	void updateVerticalMove(float elapsedTime);

	//水平移動更新処理
	void updateHorizontalVelocity(float elapsedTime);

	//水平移動処理
	void updateHorizontalMove(float elapsedTime);
private:
	float moveSpeed = 5.0f;
	float turnSpeed = 6.28f;

	float gravity = -1.0f;
	//摩擦
	float friction = 0.5f;
	//加速度
	float acceleration = 1.0f;
	//最大移動速度
	float maxMoveSpeed = 5.0f;
	// 移動方向
	DirectX::XMFLOAT3 direction = { 0,0,0 };   

	//速度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	std::function<bool(DirectX::XMFLOAT3,DirectX::XMFLOAT3,HitResult&)>raycast;
};