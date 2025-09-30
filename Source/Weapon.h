#pragma once
#include"Component/Component.h"

class Weapon : public Component
{
public:
	Weapon(Object&o);
	~Weapon();

	const char* getName() const override { return "Weapon"; }

	void OnGUI()override;

	void prepare() override;

	void update(float elapsedTime)override;
private:
	Object* owner;
	int ownerMeshIndex = -1;
	int ownerBoneIndex = -1;
};