#pragma once
#include<memory>
#include<vector>
#include<set>
#include<string>
#include<DirectXMath.h>
#include"Graphics/SkinnedMesh.h"
#include"../Utils/Singleton.h"
#include <d3d11.h>
#include"Graphics/Sprite.h"
#include"Scene.h"

#include<iostream>


class Component;
class Object : public std::enable_shared_from_this<Object>
{
public:
	Object(){}
	virtual ~Object() {};

	virtual void prepare();

	virtual void update(float elapsedTime);

	//�s��X�V����
	virtual void updateTransform();

	// GUI�\��
	virtual void onGUI();

	// ���O�̐ݒ�
	void setName(const char* name) { this->name = name; }

	// ���O�̎擾
	const char* getName() const { return name.c_str(); }

	//�ʒu�擾
	const DirectX::XMFLOAT3* getPosition()const { return &position; }
	//�ʒu�ݒ�
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	// ��]�̐ݒ�
	void setRotation(const DirectX::XMFLOAT4& rotation) { this->rotation = rotation; }

	// ��]�̎擾
	const DirectX::XMFLOAT4* getRotation() const { return &rotation; }

	void setRange(float r) { range = r; }
	float getRange() { return range; }

	void setHeight(float h) { height = h; }
	float getHeight() { return height; }

	//�g��k���擾
	const DirectX::XMFLOAT3* getScale()const { return &scale; }
	//�g��k���ݒ�
	void setScale(DirectX::XMFLOAT3& s) { this->scale = s; }

	//�g�����X�t�H�[���̎擾
	const DirectX::XMFLOAT4X4* getTransform()const { return &transform; }

	//�g�����X�t�H�[���̐ݒ�
	void setTransform(DirectX::XMFLOAT4X4 transform) { this->transform = transform; }

	//���f���ǂݍ���
	void loadModel(const char* filename);

	//���f���̎擾
	SkinnedMesh* getModel() { return model.get(); }

	// �R���|�[�l���g�ǉ�
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args&&... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->setObject(shared_from_this());
		components.emplace_back(component);
		return component;
	}

	// �R���|�[�l���g�擾
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (std::shared_ptr<Component>& component : components)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}

	DirectX::XMFLOAT4 getMaterialColor() { return materialColor; }
	void setMaterialColor(DirectX::XMFLOAT4 color) { materialColor = color; }
private:
	std::string name;
	DirectX::XMFLOAT3 position = { 0,0,0 }; //�ʒu
	DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 };//��]
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//�g��k��

	float height = 1.0; //����
	float range = 1.0f; //���a

	DirectX::XMFLOAT4 materialColor = { 1.0f,1.0f,1.0f,1.0f };

	// �p���s��
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//���f��
	std::unique_ptr<SkinnedMesh>model;

	std::vector<std::shared_ptr<Component>>components;
};

//�I�u�W�F�N�g�}�l�[�W���[
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();
	//�쐬
	std::shared_ptr<Object>create();

	//�폜
	void remove(std::shared_ptr<Object> object);

	//�X�V
	void update(float elapsedTime);

	//�s��X�V
	void updateTransform();

	//�`��
	void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection,const DirectX::XMFLOAT4 cameraPos);

	//�I�u�W�F�N�g�̑S�j��
	void clear();

	//�I�u�W�F�N�g����
	Object* searchObject(const char* name);
private:
	void drawLister();
	void drawDetail();
private:
	// �V�[���萔
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		cameraPosition;

		//�A�E�g���C���p
		float size = 0.05f;
		DirectX::XMFLOAT3 color = { 0.0f,0.0f,0.0f };
	};
	// �V�[���萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	//�V�F�[�_�[�p
	std::unique_ptr<Sprite>ranpTexture;

	std::vector<std::shared_ptr<Object>> prepareOject;
	std::vector<std::shared_ptr<Object>> updateObject;
	std::set<std::shared_ptr<Object>>	 selectionObject;
	std::set<std::shared_ptr<Object>>	 removeObject;

	DirectX::XMFLOAT4 lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };
	bool	hiddenLister = false;
	bool	hiddenDetail = false;

	float lineSize = 0.016f;
	DirectX::XMFLOAT3 lineColor = { 0.0f,0.0f,0.0f };
};
