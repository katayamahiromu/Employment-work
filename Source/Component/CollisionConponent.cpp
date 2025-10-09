#include"CollisionConponent.h"
#include"math/Mathf.h"
#include"system/Messenger.h"
#include"system/MessageData.h"

CollisionComponent::CollisionComponent(CollisionType type):type(type)
{
	Messenger::instance().sendData(MessageData::REGISTER_COLLISIO_COMPONENT, this);
}

CollisionComponent::~CollisionComponent()
{

}

void CollisionComponent::prepare()
{

}

void CollisionComponent::update(float elapsedTime)
{
	collisionPos.clear();

	switch (type)
	{
	case Mesh:meshCollisionSetting();break;
	case None:noneCollisionSetting();break;
	default:break;
	}
}

void CollisionComponent::setMeshName(std::string name)
{
	meshIndexArray.emplace_back(getObject()->getModel()->findMeshIndex(name.c_str()));
}

void CollisionComponent::setBoneName(std::string name)
{
	boneIndexArray.emplace_back(getObject()->getModel()->findBoneIndex(name.c_str()));
}

void CollisionComponent::meshCollisionSetting()
{
	//フレームを取得しそこからボーンに即して球のポジションを算出
	SkinnedMesh* model = getObject()->getModel();
	animation::keyframe key = model->getKeyFrame();
	if (key.nodes.size() == 0)return;

	for (auto mesh : meshIndexArray)
	{
		for (auto bone : boneIndexArray)
		{
			DirectX::XMFLOAT4X4 m = model->boneMatrix(mesh, bone, key);

			DirectX::XMFLOAT3 pos;

			//この二つの使用してない
			DirectX::XMFLOAT3 scale;
			DirectX::XMFLOAT4 rotation;

			Mathf::transformDecomposition(m, pos, scale, rotation);
			collisionPos.emplace_back(pos);
		}
	}
}

void CollisionComponent::noneCollisionSetting()
{
	collisionPos.emplace_back(*getObject()->getPosition());
}