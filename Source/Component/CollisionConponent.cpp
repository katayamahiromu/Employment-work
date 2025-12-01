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
	switch (type)
	{
	case Mesh:meshCollisionSetting();break;
	case None:noneCollisionSetting();break;
	default:break;
	}
}

void CollisionComponent::OnGUI()
{
	for (auto info : collisionSphereInfoArray)
	{
		ImGui::InputFloat3("Collision Pos", &info.pos.x);
		ImGui::InputFloat("size", &info.size);
	}
}

void CollisionComponent::setMeshName(std::string name)
{
	meshIndexArray.emplace_back(getObject()->getModel()->findMeshIndex(name.c_str()));
}

void CollisionComponent::setBoneInfo(std::string name, float size)
{
	boneIndexArray.emplace_back(getObject()->getModel()->findBoneIndex(name.c_str()));
	collisionSphereInfo info;
	info.pos = { 0.0f,0.0f,0.0f };
	info.size = size;
	collisionSphereInfoArray.emplace_back(info);
}

void CollisionComponent::meshCollisionSetting()
{
	//フレームを取得しそこからボーンに即して球のポジションを算出
	SkinnedMesh* model = getObject()->getModel();
	animation::keyframe key = model->getKeyFrame();
	if (key.nodes.size() == 0)return;

	//モデルのワールド行列

	//コリジョン情報にアクセスするインデックス
	int index = 0;

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
			collisionSphereInfoArray.at(index).pos = 
			DirectX::XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43);
			index++;
		}
	}
}

void CollisionComponent::noneCollisionSetting()
{
	collisionSphereInfoArray.at(0).pos = *getObject()->getPosition();
}