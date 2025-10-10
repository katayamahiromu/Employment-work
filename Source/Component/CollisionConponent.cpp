#include"CollisionConponent.h"
#include"math/Mathf.h"
#include"system/Messenger.h"
#include"system/MessageData.h"
#include"../Utils/imgui/imgui.h"

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

void CollisionComponent::OnGUI()
{
	for (auto pos : collisionPos)
	{
		ImGui::InputFloat3("Collision Pos", &pos.x);
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

	//モデルのワールド行列
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(getObject()->getTransform());
	for (auto mesh : meshIndexArray)
	{
		for (auto bone : boneIndexArray)
		{
			//モデルのローカル行列
			DirectX::XMFLOAT4X4 m = model->boneMatrix(mesh, bone, key);
			DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);

			//ボーンの位置のワールド行列を算出
			DirectX::XMMATRIX WM = M * world;
			DirectX::XMFLOAT4X4 worldMatrix;
			DirectX::XMStoreFloat4x4(&worldMatrix, WM);

			//位置の保存
			collisionPos.emplace_back(DirectX::XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43));
		}
	}
}

void CollisionComponent::noneCollisionSetting()
{
	collisionPos.emplace_back(*getObject()->getPosition());
}