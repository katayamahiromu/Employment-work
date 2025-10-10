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

	//�L�[�̊��蓖��
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

	//�ǂ̃L�[�Ɋ֐���o�^���邩
	void registerFunc(Key func, keyAllocation key) { keys[static_cast<int>(key)] = bind(func); }

	//�L�[�������ꑱ���Ă���
	bool isButton(GamePadButton button) { return (gamePad->getButton() & (button)); }

	//�L�[��������Ă���
	bool isButtonDown(GamePadButton button) { return (gamePad->getButtonDown() & (button)); }

	//�L�[�������ꂽ
	bool isButtonRelease(GamePadButton button) { return (gamePad->getButtonUp() & (button)); }
	
	//���X�e�B�b�N�̓��͒l�����
	const DirectX::XMFLOAT2& getLeftStick()const { return DirectX::XMFLOAT2(gamePad->getAxisLX(), gamePad->getAxisLY()); }

	void OnGUI()override;
private:
	//�֐����s
	void execution(GamePadButton button, keyAllocation key);
private:
	Key keys[static_cast<int>(keyAllocation::MAX)];
	GamePad* gamePad;
};