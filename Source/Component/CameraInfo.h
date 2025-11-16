#pragma once
#include"Component.h"
#include"Camera.h"

//ƒJƒƒ‰‚Ìî•ñ‚ğ•Û‘¶
class CameraInfo : public Component
{
public:
	CameraInfo(Camera* camera);
	~CameraInfo();

	const char* getName() const override { return "CameraInfo"; }

	void prepare() override;
	void update(float elapsedTime)override;
	void OnGUI()override;

	Camera* getCamera() { return camera; }
private:
	Camera* camera = nullptr;
};