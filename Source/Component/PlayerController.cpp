#include"PlayerController.h"
#include"../Macro.h"
#include"../imgui/imgui.h"

PlayerController::PlayerController()
{
	gamePad = InputManager::instance()->getGamePad();
}

PlayerController::~PlayerController()
{

}

void PlayerController::update(float elapsedTime)
{
	execution(GamePad::BTN_A, keyAllocation::key_A);
	execution(GamePad::BTN_B, keyAllocation::key_B);
	execution(GamePad::BTN_X, keyAllocation::key_X);
	execution(GamePad::BTN_Y, keyAllocation::key_Y);
}

void PlayerController::execution(GamePadButton button, keyAllocation key)
{
	//�{�^����������Ă��邩
	if (!isButton(button))return;
	//�֐����o�^����Ă���Ύ��s
	if (keys[CAST_INT(key)])keys[CAST_INT(key)]();
}

void PlayerController::OnGUI()
{
}
