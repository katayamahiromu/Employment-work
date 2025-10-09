#include"object.h"
#include"Component.h"
#include"../imgui/imgui.h"
#include"DeviceManager.h"
#include<algorithm>
#include"Graphics/Buffer.h"
#include"Graphics/GraphicsManager.h"
#include"Graphics/Texture.h"

void Object::prepare()
{
	for (std::shared_ptr<Component>& conponent : components)
	{
		conponent->prepare();
	}
}

void Object::update(float elapsedTime)
{
	for (std::shared_ptr<Component>& conponent : components)
	{
		conponent->update(elapsedTime);
	}
}

void Object::updateTransform()
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//�v�Z�������[���h�s���transform�Ɏ��o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Object::onGUI()
{
	// ���O
	{
		char buffer[1024];
		::strncpy_s(buffer, sizeof(buffer), getName(), sizeof(buffer));
		if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setName(buffer);
		}
	}

	// �g�����X�t�H�[��
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat3("Position", &position.x);
		ImGui::InputFloat4("rotation", &rotation.x);
		ImGui::InputFloat3("scale", &scale.x);

		ImGui::InputFloat("range", &range);
		ImGui::InputFloat("height", &height);
	}

	// �R���|�[�l���g
	for (std::shared_ptr<Component>& component : components)
	{
		ImGui::Spacing();
		ImGui::Separator();

		if (ImGui::CollapsingHeader(component->getName(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::PushID(StringToHash(component->GetName()));
			component->OnGUI();
			//ImGui::PopID();
		}
	}
}

void Object::loadModel(const char*filename)
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), filename);
}


ObjectManager::ObjectManager()
{

	ID3D11Device* device = DeviceManager::instance()->getDevice();
	// �V�[���萔�o�b�t�@�̍쐬
	createBuffer<ObjectManager::SceneConstants>(device, buffer.GetAddressOf());

	//�V�F�[�_�[�p
	ranpTexture = std::make_unique<Sprite>(device, L"Resources/Image/ramp.png");
}

ObjectManager::~ObjectManager()
{
	
}


std::shared_ptr<Object> ObjectManager::create()
{
	std::shared_ptr<Object> object = std::make_shared<Object>();
	{
		static int id = 0;
		char name[256];
		::sprintf_s(name, sizeof(name), "object%d", id++);
		object->setName(name);
	}
	prepareOject.emplace_back(object);
	return object;
}

//�폜
void ObjectManager::remove(std::shared_ptr<Object> object)
{
	removeObject.insert(object);
}

void ObjectManager::clear()
{
	prepareOject.clear();
	updateObject.clear();
	removeObject.clear();
	selectionObject.clear();
}

//�X�V
void ObjectManager::update(float elapsedTime)
{
	for (std::shared_ptr<Object>& obj : prepareOject)
	{
		obj->prepare();
		updateObject.emplace_back(obj);
	}
	prepareOject.clear();

	for (std::shared_ptr<Object>& obj : updateObject)
	{
		obj->update(elapsedTime);
	}

	for (const auto& obj : removeObject)
	{
		//std::erase��C++20�̃��\�b�h
		std::erase(prepareOject, obj);
		std::erase(updateObject, obj);
		selectionObject.erase(obj); // std::set �ɑ΂��Ă� std::erase �ŏ\��
	}
	removeObject.clear();

	//�s��X�V
	updateTransform();
}

//�s��X�V
void ObjectManager::updateTransform()
{
	for (std::shared_ptr<Object>& obj : updateObject)
	{
		obj->updateTransform();
	}
}

//�`��
void ObjectManager::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4 cameraPos)
{

	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	GraphicsManager* graphics = GraphicsManager::instance();

	// 3D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::MODEL)].Get());
		}
	);
	// 3D ���f���̕`��ɕK�v�ȏ��
	SceneConstants sc;

	// ���C�g�����i�������j
	sc.lightDirection = lightDirection;
	DirectX::XMStoreFloat4x4(&sc.viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	sc.cameraPosition = cameraPos;
	sc.color = lineColor;
	sc.size = lineSize;
	// 3D �`��ɗ��p����萔�o�b�t�@�̍X�V�Ɛݒ�
	bindBuffer(dc, CBS_SCENE_CONSTANTS, buffer.GetAddressOf(), & sc);

	ID3D11ShaderResourceView* srv = ranpTexture->getShaderResourceView().Get();
	dc->PSSetShaderResources(3, 1, &srv);

	for (std::shared_ptr<Object>& obj : updateObject)
	{
		SkinnedMesh* model = obj->getModel();
		if (model)
		{
			model->render(dc,*obj->getTransform(),obj->getMaterialColor());
		}
	}

	//���X�^�[�`��
	drawLister();

	//�ڍ�
	drawDetail();
}

void ObjectManager::drawLister()
{
	ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	hiddenLister = !ImGui::Begin("Actor Lister", nullptr, ImGuiWindowFlags_None);
	if (!hiddenLister)
	{
		for (std::shared_ptr<Object>& obj : updateObject)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

			if (selectionObject.find(obj) != selectionObject.end())
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			ImGui::TreeNodeEx(obj.get(), nodeFlags, obj->getName());

			if (ImGui::IsItemClicked())
			{
				// �P��I�������Ή����Ă���
				ImGuiIO& io = ImGui::GetIO();
				selectionObject.clear();
				selectionObject.insert(obj);
			}

			ImGui::TreePop();
		}
		ImGui::SliderFloat4("light direction", &lightDirection.x, -1.0f, 1.0);
		ImGui::SliderFloat("line Size", &lineSize, 0.001f, 0.05f);
		ImGui::ColorEdit3("line Color", &lineColor.x);
	}
	ImGui::End();
}


void ObjectManager::drawDetail()
{
	ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	hiddenDetail = !ImGui::Begin("Object Detail", nullptr, ImGuiWindowFlags_None);
	if (!hiddenDetail)
	{
		std::shared_ptr<Object> lastSelected = selectionObject.empty() ? nullptr : *selectionObject.rbegin();
		if (lastSelected != nullptr)
		{
			lastSelected->onGUI();
		}
	}
	ImGui::End();
}

Object*ObjectManager::searchObject(const char* name)
{ 
	for (std::shared_ptr<Object>& obj : updateObject)
	{
		if (strcmp(name, obj->getName()) == 0)
			return obj.get();
	}
	return nullptr;
}