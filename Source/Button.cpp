#include"Button.h"
#include"DeviceManager.h"

Button::Button(const wchar_t* filename)
{
	sprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), filename);
}

Button::~Button()
{

}

void Button::loadReplaceSprite(const wchar_t* filename)
{
	replace = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), filename);
}

void Button::draw()
{
	Sprite* execution = replaceFlag ? replace.get() : sprite.get();
	execution->render(DeviceManager::instance()->getDeviceContext(), pos.x, pos.y, size.x, size.y);
}