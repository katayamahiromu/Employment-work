#include"Weapon.h"
#include"math/Mathf.h"
#include"imgui.h"

Weapon::Weapon(Object& o) :owner(&o)
{

}

Weapon::~Weapon()
{

}

void Weapon::OnGUI()
{
	DirectX::XMFLOAT3 p = *getObject()->getPosition();
	ImGui::InputFloat3("position", &p.x);
}

void Weapon::prepare()
{
	ownerMeshIndex = owner->getModel()->findMeshIndex("NIC:full_body");
	ownerBoneIndex = owner->getModel()->findBoneIndex("NIC:Cup_L_BK", ownerMeshIndex);
}

void Weapon::update(float elapsedTime)
{
	Object* weapon = getObject().get();

	//�A�^�b�`�����g������{�[���̍s��
	animation::keyframe key = owner->getModel()->getKeyFrame();
	if (key.nodes.size() == 0)return;
	DirectX::XMFLOAT4X4 m = owner->getModel()->boneMatrix(ownerMeshIndex, ownerBoneIndex, key);
	DirectX::XMMATRIX TargetMatrix = DirectX::XMLoadFloat4x4(&m);

	//�I�[�i�[�̍s��
	DirectX::XMMATRIX OwnerWorld = DirectX::XMLoadFloat4x4(owner->getTransform());

	DirectX::XMMATRIX weaponMat = TargetMatrix * OwnerWorld;

	DirectX::XMFLOAT4X4 weaponMatrix;
	DirectX::XMStoreFloat4x4(&weaponMatrix, weaponMat);

	weapon->setTransform(weaponMatrix);

	//�s������Ɉʒu�A��]�̐ݒ�
	/*DirectX::XMVECTOR scaleVec, rotQuat, transVec;
	DirectX::XMMatrixDecompose(&scaleVec, &rotQuat, &transVec, weaponMat);*/

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;

	Mathf::transformDecomposition(weaponMatrix, pos, scale, rotation);

	//���ꂼ��̐ݒ�
	weapon->setPosition(pos);
	weapon->setScale(scale);
	weapon->setRotation(rotation);
}