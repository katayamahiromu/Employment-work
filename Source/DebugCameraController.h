#pragma once
#include"Camera.h"
#include<memory>

class DebugCameraController
{
public:
	DebugCameraController();
	~DebugCameraController();

	void update(float elapsedTime);

	Camera* getCamera() { return camera.get(); }
private:
	std::unique_ptr<Camera>camera;
	//デバック用
	float rotateX = DirectX::XMConvertToRadians(45);
	float rotateY = 0.0f;
	float distance = 10.0f;
	DirectX::XMFLOAT3 camera_position{ 0.0f, 0.0f, -10.0f };
	DirectX::XMFLOAT3 camera_focus{ 0.0f, 0.0f, 0.0f };
};