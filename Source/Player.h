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

//プレイヤー
class Player :public Component
{
public:
	Player();
	~Player()override;

	const char* getName() const override { return "Player"; }

	void OnGUI()override;

	void prepare() override;

	void update(float elapsedTime)override;

	//入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 getMoveVec()const;
private:

	//移動入力処理
	bool inputMove(float elapsedTime);
	//攻撃入力処理
	bool inputAttack();

	//プレイヤーと敵の押し出し
	void playerVSEnemy(void*data);

	//デバック
	void debugDrawPrimitive();

	//カメラにデータを送信する
	void sendCameraData();

	//ポストエフェクトにリワインドするか送る
	void createRewindTime();
private:
	//待機ステート
	void enterIdle();
	void executeIdle(float elapsedTime);

	//移動ステート
	void enterMove();
	void executeMove(float elapsedTime);

	//攻撃ステート
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

	//エフェクト用
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