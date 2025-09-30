#include"StageManager.h"
#include"stage.h"
#include"system/MessageData.h"
#include"system/Messenger.h"

StageManager::StageManager(ObjectManager& om):om(&om)
{
	//一旦メインステージの作成
	CreateMainStage();

	RAY_CAST_REQUEST_KEY = Messenger::instance().addReceiver(MessageData::RAY_CAST_RESULT, [&](void* data) { rayCastRequest(data); });
}

StageManager::~StageManager()
{
	Messenger::instance().removeReceiver(RAY_CAST_REQUEST_KEY);
	for (auto obj : stageArray)
	{
		obj.reset();
	}
	stageArray.clear();
}

void StageManager::CreateMainStage()
{
	std::shared_ptr<Object>obj = om->create();
	obj->loadModel("Resources\\Model\\ExampleStage\\ExampleStage.fbx");
	obj->setName("main stage");
	obj->AddComponent<Stage>();

	stageArray.emplace_back(obj);
}

bool StageManager::raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	for (const auto obj : stageArray)
	{
		if (obj->GetComponent<Stage>()->raycast(start, end, hit))
		{
			return result = true;
		}
	}

	return result;
}

void StageManager::rayCastRequest(void* data)
{
	//メッセージの受け取り
	MessageData::RAYCASTREQUEST* r = static_cast<MessageData::RAYCASTREQUEST*>(data);
	for (const auto obj : stageArray)
	{
		if (obj->GetComponent<Stage>()->raycast(r->start, r->end, r->result))
		{
			r->isHit = true;
			break;
		}
	}
}