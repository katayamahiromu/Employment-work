#pragma once
#include"Component/Component.h"
#include"Component/Animation.h"
#include"Component/Movement.h"
#include"Component/TimeLapse.h"
#include"Component/Audio3DEmitter.h"
#include"Component/CollisionConponent.h"

#include"system/StateMachine.h"

class Enemy : public Component
{
public:
	Enemy(Object&owner);
	~Enemy()override;

	const char* getName() const override { return "Enemy"; }

	void OnGUI()override;

	void prepare() override;

	void update(float elapsedTime)override;
private:
	//待機
	void enterIdle();
	void executeIdle(float elapsedTime);

	//追跡
	void enterPursuit();
	void executePursuit(float elapsedTime);

	//攻撃
	void enterAttack();
	void executeAttack(float elapsedTime);
private:
	DirectX::XMFLOAT3 getMoveVec();
	void Move(float elapsedTime);
	void setRandomTargetPosition();
	void drawDebugPrimitive();
private:
	DirectX::XMFLOAT3	targetPosition = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	territoryOrigin = { 0.0f,0.0f,0.0f };
	float				territoryRange = 10.0f;
private:
	Object* owner;
	std::shared_ptr<Movement>movement;
	std::shared_ptr<Animation>animation;
	std::shared_ptr<TimeLapse>timeLaps;
	std::shared_ptr<Audio3DEmitter>emitter;
	std::shared_ptr<CollisionComponent>collision;
	std::unique_ptr<StateMachine>stateMachine;

	std::vector<DirectX::XMFLOAT3>receiveCollision;
	//アニメーション用
	enum class Anime
	{
		Idle,
		Fwd_Start,
		Fwd_Walk,
		Fwd_Stop,
		Attack,
		Damage,
		Death
	};

	//行動用
	enum class Action
	{
		Idle,
		Pursuit,
		Attack,
	};
};