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
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行列をtransformに取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Object::onGUI()
{
	// 名前
	{
		char buffer[1024];
		::strncpy_s(buffer, sizeof(buffer), getName(), sizeof(buffer));
		if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setName(buffer);
		}
	}

	// トランスフォーム
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat3("Position", &position.x);
		ImGui::InputFloat4("rotation", &rotation.x);
		ImGui::InputFloat3("scale", &scale.x);

		ImGui::InputFloat("range", &range);
		ImGui::InputFloat("height", &height);
	}

	// コンポーネント
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
	// シーン定数バッファの作成
	createBuffer<ObjectManager::SceneConstants>(device, buffer.GetAddressOf());

	//シェーダー用
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

//削除
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

//更新
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
		//std::eraseはC++20のメソッド
		std::erase(prepareOject, obj);
		std::erase(updateObject, obj);
		selectionObject.erase(obj); // std::set に対しては std::erase で十分
	}
	removeObject.clear();

	//行列更新
	updateTransform();
}

//行列更新
void ObjectManager::updateTransform()
{
	for (std::shared_ptr<Object>& obj : updateObject)
	{
		obj->updateTransform();
	}
}

//描画
void ObjectManager::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4 cameraPos)
{

	ID3D11DeviceContext* dc = DeviceManager::instance()->getDeviceContext();
	GraphicsManager* graphics = GraphicsManager::instance();

	// 3D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::MODEL)].Get());
		}
	);
	// 3D モデルの描画に必要な情報
	SceneConstants sc;

	// ライト方向（下方向）
	sc.lightDirection = lightDirection;
	DirectX::XMStoreFloat4x4(&sc.viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	sc.cameraPosition = cameraPos;
	sc.color = lineColor;
	sc.size = lineSize;
	// 3D 描画に利用する定数バッファの更新と設定
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

	//リスター描画
	drawLister();

	//詳細
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
				// 単一選択だけ対応しておく
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