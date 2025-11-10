#pragma once
#include<DirectXMath.h>

//カメラ操作
class CameraController
{
public:
	CameraController();
	~CameraController();

	//更新処理
	void update(float elapsedTime);

	void OnGui();

	void setFrontY(float frontY) { this->frontY = frontY; }

	void debugUpdate(float elapsedTime);
private:
	//フリーモード
	void freeCamera(float elapsedTime);

	//ロックオン時
	void lockonCamera(float elapsedTime);

	//実際に来てほいEyeの位置
	void computeEye(float elapsedTime);
	void computeEyeTPS(float elapsedTime);

	//カメラの回転
	void rotateCameraAngle(float elapsedTime);

	//通常のカメラ
	void onFreeMode(void* data);

	//ロックオンカメラ
	void onLockonMode(void* data);

	//横軸のズレを算出
	float calcSide(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);
private:
	//モード
	enum Mode
	{
		free,
		lockon,
	};

	Mode mode = Mode::free;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 newPosition = { 0,0,0 };

	DirectX::XMFLOAT3 target = { 0,0,0 };
	DirectX::XMFLOAT3 newTarget = { 0,0,0 };
	DirectX::XMFLOAT3 angle  = { 0,0,0 }; //カメラの角度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float frontY = 0.0f;
	float range = 5.0f;
	float spring = 10.0;
	float damping = 2.0;

	// 自動回転処理
	float targetAngleY = 0;

	//メッセージキー
	uint64_t CAMERACHANGEFREEMODEKEY;
	uint64_t CAMERACHANGELOCKONMODEKEY;

	// ロックオン処理
	DirectX::XMFLOAT3	targetWork[2] = { { 0, 0, 0 }, { 0, 0, 0 } };	// 0 : 座標, 1 : 注視点
	float				lengthLimit[2] = { 5, 7 };
	float				sideValue = 1;

	//デバック用
	float rotateX = DirectX::XMConvertToRadians(45);
	float rotateY = 0.0f;
	float distance = 10.0f;
	DirectX::XMFLOAT3 camera_position{ 0.0f, 0.0f, -10.0f };
	DirectX::XMFLOAT3 camera_focus{ 0.0f, 0.0f, 0.0f };
};