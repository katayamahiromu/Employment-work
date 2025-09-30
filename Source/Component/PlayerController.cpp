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
	//ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚Ä‚¢‚é‚©
	if (!isButton(button))return;
	//ŠÖ”‚ª“o˜^‚³‚ê‚Ä‚¢‚ê‚ÎÀs
	if (keys[CAST_INT(key)])keys[CAST_INT(key)]();
}

void PlayerController::OnGUI()
{
}
