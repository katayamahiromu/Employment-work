#include"Enemy.h"
#include"../Macro.h"
#include"math/Mathf.h"
#include"Graphics/GraphicsManager.h"
#include"system/Messenger.h"
#include"system/MessageData.h"

Enemy::Enemy(Object&owner):owner(&owner)
{
	stateMachine = std::make_unique<StateMachine>();
}

Enemy::~Enemy()
{

}

void Enemy::OnGUI()
{

}

void Enemy::prepare()
{
	movement = getObject()->GetComponent<Movement>();
	movement->setMoveSpeed(3.0f);

	animation = getObject()->GetComponent<Animation>();
	timeLaps = getObject()->GetComponent<TimeLapse>();
	emitter = getObject()->GetComponent<Audio3DEmitter>();
	emitter->loadFile("Resources\\Audio\\グレート.wav");

	collision = getObject()->GetComponent<CollisionComponent>();
	collision->setMeshName("PLT:PLT_mesh");
	collision->setBoneName("PLT:Knee_L_BK");
	collision->setBoneName("PLT:Knee_R_BK");
	
	//待機
	stateMachine->registerState(CAST_INT(Action::Idle),
		new State(FUNC_SET_0(enterIdle), FUNC_SET_FLOAT(executeIdle), nullptr));

	//追跡
	stateMachine->registerState(CAST_INT(Action::Pursuit),
		new State(FUNC_SET_0(enterPursuit), FUNC_SET_FLOAT(executePursuit), nullptr));

	//攻撃
	stateMachine->registerState(CAST_INT(Action::Attack),
		new State(FUNC_SET_0(enterAttack), FUNC_SET_FLOAT(executeAttack), nullptr));

	stateMachine->changeState(CAST_INT(Action::Idle));

	targetPosition = *owner->getPosition();
}

void Enemy::update(float elapsedTime)
{
	receiveCollision.clear();

	stateMachine->update(elapsedTime);

	//エミターの更新
	emitter->play(static_cast<int>(Lisner::PLAYER));
	emitter->setVelocity(movement->getVelocity());

	//当たり判定用にプレイヤーに情報を送信
	drawDebugPrimitive();

	//プレイヤーに押し出し用情報を送る
	MessageData::CylinderInfo ci = { *getObject()->getPosition(),2.0f, 5.0f };
	Messenger::instance().sendData(MessageData::CYLINDER_INFORMATION, &ci);
}

DirectX::XMFLOAT3 Enemy::getMoveVec()
{
	//ターゲットへの進行ベクトルを算出
	DirectX::XMVECTOR Vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&targetPosition), DirectX::XMLoadFloat3(getObject()->getPosition())));

	DirectX::XMFLOAT3 vec;
	DirectX::XMStoreFloat3(&vec, Vec);
	return vec;
}

void Enemy::Move(float elapsedTime)
{
	DirectX::XMFLOAT3 vec = getMoveVec();
	vec.y = 0.0f;
	movement->move(vec,elapsedTime);
	movement->turn(vec, elapsedTime);
}

void Enemy::setRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

void Enemy::enterIdle()
{
	animation->playAnimation(CAST_INT(Anime::Idle), false);
}

void Enemy::executeIdle(float elapsedTime)
{
	if (!animation->isPlayAnimation())
	{
		stateMachine->changeState(CAST_INT(Action::Pursuit));
	}
}

void Enemy::enterPursuit()
{
	animation->playAnimation(CAST_INT(Anime::Fwd_Walk), true);
	targetPosition = *owner->getPosition();
}

void Enemy::executePursuit(float elapsedTime)
{
	Move(elapsedTime);
	float distsq = Mathf::calcDistanceSqXZ(targetPosition, *getObject()->getPosition());
	if (distsq < 0.25)
	{
		stateMachine->changeState(CAST_INT(Action::Attack));
	}
}

void Enemy::enterAttack()
{
	animation->playAnimation(CAST_INT(Anime::Attack),false);
}

void Enemy::executeAttack(float elapsedTime)
{
	if (!animation->isPlayAnimation())
	{
		stateMachine->changeState(CAST_INT(Action::Idle));
	}
}

void Enemy::drawDebugPrimitive()
{
	GraphicsManager::instance()->getDebugRenderer()->drawCylinder(*getObject()->getPosition(), 2.0f, 5.0f,{0,0,0,1});
}