#pragma once

#include <d3d11.h>
class UI
{
public:
	UI() {};
	virtual ~UI(){}
	virtual void update(float elapsedTime) = 0;
	virtual void render(ID3D11DeviceContext* dc) = 0;
};