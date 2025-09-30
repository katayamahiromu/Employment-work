#pragma once
#include"Particle.h"
#include<list>
#include<set>

class ParticleManager
{
public:
	ParticleManager() {};
	~ParticleManager() {clear(); };
	

	void regist(Particle* p) { particleList.emplace_back(p); }
	void clear();
	void update(float elapsedTime);
	void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

private:
	std::list<Particle*>particleList;
};