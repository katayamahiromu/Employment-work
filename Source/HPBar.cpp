#include"HPBar.h"
#include"DeviceManager.h"

HPBar::HPBar(float* value):manageValue(value), initializeValue(*value)
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	baseLine = std::make_unique<Sprite>(device, nullptr);
	gage = std::make_unique<Sprite>(device, nullptr);
	gageWidth = MaxGageWidth;
}

HPBar::~HPBar()
{

}

void HPBar::update(float elapsedTime)
{
	gageWidth = (*manageValue / initializeValue) * MaxGageWidth;
}

void HPBar::render(ID3D11DeviceContext* dc)
{
	baseLine->render(dc, { 100.0f, 10.0f, 1080.0f, 30.f }, { 0.2f,0.2f,0.2f,0.8f });
	gage->render(dc, {100.0f,14.0f,gageWidth,22.0f}, { 0.0f, 1.0f, 0.0f, 1.0f });
}