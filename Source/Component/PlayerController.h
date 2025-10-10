#pragma once
#include"Component.h"
#include"../Input/InputManager.h"
#include<functional>

using Key = std::function<void()>;

class PlayerController :public Component
{
public:
	PlayerController();
	~PlayerController();

	const char* getName() const override { return "Player Controller"; }

	void update(float elapsedTime)override;

	//キーの割り当て
	enum class keyAllocation :int
	{
		key_Y,
		key_X,
		key_A,
		key_B,
		key_LB,
		key_RB,
		key_LT,
		key_RL,
		MAX
	};

	//どのキーに関数を登録するか
	void registerFunc(Key func, keyAllocation key) { keys[static_cast<int>(key)] = bind(func); }

	//キーが押され続けている
	bool isButton(GamePadButton button) { return (gamePad->getButton() & (button)); }

	//キーが押されている
	bool isButtonDown(GamePadButton button) { return (gamePad->getButtonDown() & (button)); }

	//キーが離された
	bool isButtonRelease(GamePadButton button) { return (gamePad->getButtonUp() & (button)); }
	
	//左スティックの入力値を入手
	const DirectX::XMFLOAT2& getLeftStick()const { return DirectX::XMFLOAT2(gamePad->getAxisLX(), gamePad->getAxisLY()); }

	void OnGUI()override;
private:
	//関数実行
	void execution(GamePadButton button, keyAllocation key);
private:
	Key keys[static_cast<int>(keyAllocation::MAX)];
	GamePad* gamePad;
};