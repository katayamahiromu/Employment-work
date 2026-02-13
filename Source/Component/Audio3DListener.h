#pragma once
#include"Component.h"
#include"Audio/Audio3DSystem.h"

class Audio3DListener:public Component
{
public:
	Audio3DListener(int slot);
	~Audio3DListener();


	const char* getName() const override { return "Audio3DLister"; }

	void prepare() override;
	void update(float elapsedTime)override;
	void OnGUI()override;

	//速度の更新
	void setVelocity(DirectX::XMFLOAT3 v) { listener.velocity = v; }
	
	//位置更新
	void setPosition(DirectX::XMFLOAT3 p) { listener.position = p; }
private:
	SoundListner listener;
};