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

    //�ҋ@
    stateMachine->registerState(
        CAST_INT(Action::Idel),
        new State(FUNC_SET_0(enterIdle), FUNC_SET_FLOAT(executeIdle), nullptr));

    //�ړ�
    stateMachine->registerState(
        CAST_INT(Action::Run),
        new State(FUNC_SET_0(enterMove), FUNC_SET_FLOAT(executeMove), nullptr));

    //�U��
    stateMachine->registerState(
        CAST_INT(Action::Attack),
        new State(FUNC_SET_0(enterAttack), FUNC_SET_FLOAT(executeAttack), nullptr));

    //�����X�e�[�g�ݒ�
    stateMachine->changeState(CAST_INT(Action::Idel));
}

void Player::update(float elapsedTime)
{
    stateMachine->update(elapsedTime);

    //���X�i�[�̏��X�V
    lister->setVelocity(movement->getVelocity());

    //�{�^����������Ă��邩�̏�Ԃ𑗐M
    TimeLapseManager::instance().setIsRecord(playerController->isButton(GamePad::BTN_A));
    TimeLapseManager::instance().setIsPushButton(playerController->isButtonDown(GamePad::BTN_A));
    TimeLapseManager::instance().setIsRelease(playerController->isButtonRelease(GamePad::BTN_A));

    //�J�����ɏ��𑗂�
    sendCameraData();

    debugDrawPrimitive();

    //�����߂����̃G�t�F�N�g����
    createRewindTime();
}

void Player::sendCameraData()
{
    //�J�����ɏ��𑗂�
    std::shared_ptr<Object> o = getObject();
    MessageData::CAMERACHANGEFREEMODEDATA p = { *o->getPosition(),*o->getRotation() };
    p.target.y += 1.0f;
    p.angle.y = Mathf::getYawFromQuaternion(p.angle);
    Messenger::instance().sendData(MessageData::CAMERACHANGEFREEMODE, &p);
}

void Player::playerVSEnemy(void* data)
{
    //���b�Z�[�W�̎󂯎��
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
    // ���͏����擾
    DirectX::XMFLOAT2 leftStick = playerController->getLeftStick();
    // �J�����������擾
    Camera* camera = Camera::instance();
    const DirectX::XMFLOAT3* cameraFront = camera->getFront();
    const DirectX::XMFLOAT3* cameraRight = camera->getRight();

    // �J�����O�����x�N�g���� XZ �P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront->x;
    float cameraFrontZ = cameraFront->z;

    // �J�����O�����x�N�g����P�ʃx�N�g����
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    //�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight->x;
    float cameraRightZ = cameraRight->z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        cameraRightX = cameraRightX / cameraRightLength;
        cameraRightZ = cameraRightZ / cameraRightLength;
    }

    // �������͒l���J�����O�����ɁA�����������J�����E�����ɔ��f���i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX * leftStick.y + cameraRightX * leftStick.x;
    vec.z = cameraFrontZ * leftStick.y + cameraRightZ * leftStick.x;

    // Y �������ɂ͈ړ����Ȃ��B
    vec.y = 0.0f;

    return vec;
}

bool Player::inputMove(float elapsedTime)
{
    DirectX::XMFLOAT3 vec = getMoveVec();
    //�ړ�����
    movement->move(vec, elapsedTime);
    //��]����
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
    //�{�^����������Ă��邩
    if (playerController->isButton(GamePad::BTN_A))
    {
        //��x��������
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