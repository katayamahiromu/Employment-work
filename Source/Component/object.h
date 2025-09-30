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

	//行列更新処理
	virtual void updateTransform();

	// GUI表示
	virtual void onGUI();

	// 名前の設定
	void setName(const char* name) { this->name = name; }

	// 名前の取得
	const char* getName() const { return name.c_str(); }

	//位置取得
	const DirectX::XMFLOAT3* getPosition()const { return &position; }
	//位置設定
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	// 回転の設定
	void setRotation(const DirectX::XMFLOAT4& rotation) { this->rotation = rotation; }

	// 回転の取得
	const DirectX::XMFLOAT4* getRotation() const { return &rotation; }

	void setRange(float r) { range = r; }
	float getRange() { return range; }

	void setHeight(float h) { height = h; }
	float getHeight() { return height; }

	//拡大縮小取得
	const DirectX::XMFLOAT3* getScale()const { return &scale; }
	//拡大縮小設定
	void setScale(DirectX::XMFLOAT3& s) { this->scale = s; }

	//トランスフォームの取得
	const DirectX::XMFLOAT4X4* getTransform()const { return &transform; }

	//トランスフォームの設定
	void setTransform(DirectX::XMFLOAT4X4 transform) { this->transform = transform; }

	//モデル読み込み
	void loadModel(const char* filename);

	//モデルの取得
	SkinnedMesh* getModel() { return model.get(); }

	// コンポーネント追加
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args&&... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->setObject(shared_from_this());
		components.emplace_back(component);
		return component;
	}

	// コンポーネント取得
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
	DirectX::XMFLOAT3 position = { 0,0,0 }; //位置
	DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 };//回転
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//拡大縮小

	float height = 1.0; //高さ
	float range = 1.0f; //半径

	DirectX::XMFLOAT4 materialColor = { 1.0f,1.0f,1.0f,1.0f };

	// 姿勢行列
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//モデル
	std::unique_ptr<SkinnedMesh>model;

	std::vector<std::shared_ptr<Component>>components;
};

//オブジェクトマネージャー
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();
	//作成
	std::shared_ptr<Object>create();

	//削除
	void remove(std::shared_ptr<Object> object);

	//更新
	void update(float elapsedTime);

	//行列更新
	void updateTransform();

	//描画
	void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection,const DirectX::XMFLOAT4 cameraPos);

	//オブジェクトの全破棄
	void clear();

	//オブジェクト検索
	Object* searchObject(const char* name);
private:
	void drawLister();
	void drawDetail();
private:
	// シーン定数
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		cameraPosition;

		//アウトライン用
		float size = 0.05f;
		DirectX::XMFLOAT3 color = { 0.0f,0.0f,0.0f };
	};
	// シーン定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	//シェーダー用
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
