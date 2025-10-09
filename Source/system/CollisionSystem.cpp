#include"CollisionSystem.h"
#include"MessageData.h"
#include"Messenger.h"
#include"Graphics/GraphicsManager.h"

CollisionSystem::CollisionSystem()
{
	REGISTERKEY = Messenger::instance().addReceiver(
		MessageData::REGISTER_COLLISIO_COMPONENT, [&](void* data)
		{ registerCollisionComponent(data); });
}
CollisionSystem::~CollisionSystem()
{
	componentArray.clear();
	Messenger::instance().removeReceiver(REGISTERKEY);
}

void CollisionSystem::update()
{

}

void CollisionSystem::debugRender()
{
	DebugRenderer* dr = GraphicsManager::instance()->getDebugRenderer();
	for (auto component : componentArray)
	{
		for (auto p : component->getPositionArray())
		{
			dr->drawSphere(p, 0.5f, DirectX::XMFLOAT4(1, 1, 1, 1));
		}
	}
}

void CollisionSystem::registerCollisionComponent(void*data)
{
	if (data == nullptr)return;
	CollisionComponent* component = static_cast<CollisionComponent*>(data);
	componentArray.emplace_back(component);
}