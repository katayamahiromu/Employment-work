#pragma once

#include"Graphics/Sprite.h"
#include"particle_system.h"
#include<memory>

class Particle
{
public:
	Particle(const wchar_t* filename);
	virtual ~Particle() {};

	virtual void update(float elapsedTime) = 0;
	void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
protected:
	std::unique_ptr<Sprite>sprite;
	std::unique_ptr<ParticleSystem>particleSystem;
};

class FireParticle : public Particle
{
public:
	FireParticle() :Particle(L"Resources/Image/Explosion01_5x5.png") {}
	~FireParticle()override{}
	void update(float elapsedTime)override
	{
		if (::GetAsyncKeyState('R') & 0x8000)
		{
			//DirectX::XMFLOAT3 p = *playerManager->getPlayer()->getPosition();
			DirectX::XMFLOAT3 p = {0,0,0};
			p.y += 1.0f;
			p.z -= 0.5f;

			particleSystem->Set(
				0, 1.0f,
				p,
				DirectX::XMFLOAT3(
					(rand() % 200 - 100) * 0.001f,
					(rand() % 100) * 0.005f,0),
				DirectX::XMFLOAT3(0, 0.5f, 0),
				DirectX::XMFLOAT2(3.0f, 3.0f),
				true,
				24.0f
			);
		}
		particleSystem->Update(elapsedTime);
	}
};