#include"Component/object.h"
#include"Camera.h"
#include"imgui.h"
#include"../Utils/Macro.h"
#include"math/Mathf.h"
#include"math/Collision.h"
#include"Player.h"

#include"system/MessageData.h"
#include"system/Messenger.h"
#include"system/TimeLapseManager.h"
#include"math/Mathf.h"

#include"Graphics/GraphicsManager.h"
#include"system/PostprocessingRenderer.h"

Player::Player()
{
    stateMachine = std::make_unique<StateMachine>();

    CollisionCylinderKey = Messenger::instance().addReceiver(MessageData::CYLINDER_INFORMATION, [&](void* data) { playerVSEnemy(data); });
}

Player::~Player()
{
    if (rewindEffect)
    {
        delete rewindEffect;
        rewindEffect = nullptr;
    }
    Messenger::instance().removeReceiver(CollisionCylinderKey);
}

void Player::OnGUI()
{
}

void Player::prepare()
{
    movement = getObject()->GetComponent<Movement>();
    animation = getObject()->GetComponent<Animation>();
    timeLapse = getObject()->GetComponent<TimeLapse>();
    playerController = getObject()->GetComponent<PlayerController>();
    lister = getObject()->GetComponent<Audio3DListener>();
    collision = getObject()->GetComponent<CollisionComponent>();

    collision->setMeshName("NIC:magic_wand");
    collision->setBoneName("NIC:wand2_BK");
   

    playerController->registerFunc([]() {TimeLapseManager::instance().outputRecordInformation();}, PlayerController::keyAllocation::key_A);

    //待機
    stateMachine->registerState(
        CAST_INT(Action::Idel),
        new State(FUNC_SET_0(enterIdle), FUNC_SET_FLOAT(executeIdle), nullptr));

    //移動
    stateMachine->registerState(
        CAST_INT(Action::Run),
        new State(FUNC_SET_0(enterMove), FUNC_SET_FLOAT(executeMove), nullptr));

    //攻撃
    stateMachine->registerState(
        CAST_INT(Action::Attack),
        new State(FUNC_SET_0(enterAttack), FUNC_SET_FLOAT(executeAttack), nullptr));

    //初期ステート設定
    stateMachine->changeState(CAST_INT(Action::Idel));
}

void Player::update(float elapsedTime)
{
    stateMachine->update(elapsedTime);

    //リスナーの情報更新
    lister->setVelocity(movement->getVelocity());

    //ボタンが押されているかの状態を送信
    TimeLapseManager::instance().setIsRecord(playerController->isButton(GamePad::BTN_A));
    TimeLapseManager::instance().setIsPushButton(playerController->isButtonDown(GamePad::BTN_A));
    TimeLapseManager::instance().setIsRelease(playerController->isButtonRelease(GamePad::BTN_A));

    //カメラに情報を送る
    sendCameraData();

    debugDrawPrimitive();

    //巻き戻し時のエフェクト生成
    createRewindTime();
}

void Player::sendCameraData()
{
    //カメラに情報を送る
    std::shared_ptr<Object> o = getObject();
    MessageData::CAMERACHANGEFREEMODEDATA p = { *o->getPosition(),*o->getRotation() };
    p.target.y += 1.0f;
    p.angle.y = Mathf::getYawFromQuaternion(p.angle);
    Messenger::instance().sendData(MessageData::CAMERACHANGEFREEMODE, &p);
}

void Player::playerVSEnemy(void* data)
{
    //メッセージの受け取り
    MessageData::CylinderInfo* ci = Messenger::instance().getMessageData<MessageData::CylinderInfo>(data);
    DirectX::XMFLOAT3 outVec;
    if (Collision::intersectCylinderAndCylinder(
        ci->pos,
        ci->range,
        ci->height,
        *getObject()->getPosition(),
        getObject()->getRange(),
        getObject()->getHeight(),
        outVec
    ))
    {
        getObject()->setPosition(outVec);
    }
}

void Player::debugDrawPrimitive()
{
    DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();
    DirectX::XMFLOAT3 pos = *getObject()->getPosition();
    float height = getObject()->getHeight();
    float range = getObject()->getRange();
    debugRenderer->drawCylinder(pos, range, height, { 0,0,0,1 });
    //debugRenderer->drawSphere(pos, 1.0f, { 0,0,0,1 });
}

DirectX::XMFLOAT3 Player::getMoveVec()const
{  
    // 入力情報を取得
    DirectX::XMFLOAT2 leftStick = playerController->getLeftStick();
    // カメラ方向を取得
    Camera* camera = Camera::instance();
    const DirectX::XMFLOAT3* cameraFront = camera->getFront();
    const DirectX::XMFLOAT3* cameraRight = camera->getRight();

    // カメラ前方向ベクトルを XZ 単位ベクトルに変換
    float cameraFrontX = cameraFront->x;
    float cameraFrontZ = cameraFront->z;

    // カメラ前方向ベクトルを単位ベクトル化
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    //カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight->x;
    float cameraRightZ = cameraRight->z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        cameraRightX = cameraRightX / cameraRightLength;
        cameraRightZ = cameraRightZ / cameraRightLength;
    }

    // 垂直入力値をカメラ前方向に、水平方向をカメラ右方向に反映し進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX * leftStick.y + cameraRightX * leftStick.x;
    vec.z = cameraFrontZ * leftStick.y + cameraRightZ * leftStick.x;

    // Y 軸方向には移動しない。
    vec.y = 0.0f;

    return vec;
}

bool Player::inputMove(float elapsedTime)
{
    DirectX::XMFLOAT3 vec = getMoveVec();
    //移動処理
    movement->move(vec, elapsedTime);
    //回転処理
    movement->turn(vec, elapsedTime);

    return(vec.x != 0.0f || vec.z != 0.0f) ? true : false;
}

bool Player::inputAttack()
{
    return playerController->isButton(GamePad::BTN_B);
}

void Player::enterIdle()
{
    animation->playAnimation(CAST_INT(Anime::Idel), true);
}

void Player::executeIdle(float elapsedTime)
{
    if (inputMove(elapsedTime))
    {
        stateMachine->changeState(CAST_INT(Action::Run));
    }

    if (inputAttack())
    {
        stateMachine->changeState(CAST_INT(Action::Attack));
    }
}

void Player::enterMove()
{
    animation->playAnimation(CAST_INT(Anime::FwdRun), true);
}

void Player::executeMove(float elapsedTime)
{
    if (!inputMove(elapsedTime))
    {
        stateMachine->changeState(CAST_INT(Action::Idel));
    }
    if (inputAttack())
    {
        stateMachine->changeState(CAST_INT(Action::Attack));
    }
}

void Player::enterAttack()
{
    animation->playAnimation(CAST_INT(Anime::Attack),false);
}

void Player::executeAttack(float elapsedTime)
{
    if (!animation->isPlayAnimation())
    {
        stateMachine->changeState(CAST_INT(Action::Idel));
    }
}

void Player::createRewindTime()
{
    PostprocessingRenderer* postEffect = PostprocessingRenderer::instance();
    //ボタンを押されているか
    if (playerController->isButton(GamePad::BTN_A))
    {
        //一度だけ生成
        if (!isRewindEffect)
        {
            rewindEffect = new RewindLine;
            postEffect->addPostProcess(rewindEffect);
            isRewindEffect = true;
        }
    }
    else
    {
        if (isRewindEffect)
        {
            postEffect->backErase();
            delete rewindEffect;
            rewindEffect = nullptr;
            isRewindEffect = false;
        }
    }
}