#pragma once
#include"Component/Component.h"
#include"Component/Movement.h"
#include"Component/Animation.h"
#include"Component/TimeLapse.h"
#include"Component/PlayerController.h"
#include"Component/Audio3DListener.h"
#include"Component/CollisionConponent.h"

#include"system/StateMachine.h"
#include"Graphics/RewidLine.h"

//�v���C���[
class Player :public Component
{
public:
	Player();
	~Player()override;

	const char* getName() const override { return "Player"; }

	void OnGUI()override;

	void prepare() override;

	void update(float elapsedTime)override;

	//���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 getMoveVec()const;
private:

	//�ړ����͏���
	bool inputMove(float elapsedTime);
	//�U�����͏���
	bool inputAttack();

	//�v���C���[�ƓG�̉����o��
	void playerVSEnemy(void*data);

	//�f�o�b�N
	void debugDrawPrimitive();

	//�J�����Ƀf�[�^�𑗐M����
	void sendCameraData();

	//�|�X�g�G�t�F�N�g�Ƀ����C���h���邩����
	void createRewindTime();
private:
	//�ҋ@�X�e�[�g
	void enterIdle();
	void executeIdle(float elapsedTime);

	//�ړ��X�e�[�g
	void enterMove();
	void executeMove(float elapsedTime);

	//�U���X�e�[�g
	void enterAttack();
	void executeAttack(float elapsedTime);

	enum class Anime
	{
		Idel,
		FwdRun,
		FwdStart,
		FwdStop,
		JumpAir,
		JumpRecovery,
		JumpStart,
		Attack,
		Death,
	};

	enum class Action
	{
		Idel,
		Run,
		Jump,
		Attack
	};
private:
	uint64_t CollisionCylinderKey;

	//�G�t�F�N�g�p
	bool isRewindEffect = false;
	RewindLine* rewindEffect = nullptr;
	float effectTime = 0.0f;
private:
	std::shared_ptr<Movement>movement;
	std::shared_ptr<Animation>animation;
	std::shared_ptr<TimeLapse>timeLapse;
	std::shared_ptr<PlayerController>playerController;
	std::shared_ptr<Audio3DListener>lister;
	std::shared_ptr<CollisionComponent>collision;

	std::unique_ptr<StateMachine>stateMachine;
};