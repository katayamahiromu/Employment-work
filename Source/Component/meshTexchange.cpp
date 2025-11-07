#include"meshTexchange.h"
#include"imgui.h"
#include"Graphics/Texture.h"
#include"DeviceManager.h"

MeshTexChange::MeshTexChange(const char* materialName):materialName(materialName)
{
}

MeshTexChange::~MeshTexChange()
{

}

void MeshTexChange::prepare()
{
	SkinnedMesh* m = getObject()->getModel();

	//メッシュ内のサブセットから変更したいマテリアルキーを取得
	for (auto meshes : m->meshes)
	{
		for (const SkinnedMesh::mesh::subset& subset : meshes.subsets)
		{
			if (std::strcmp(materialName, subset.material_name.c_str()) == 0)
			{
				key = subset.material_unique_id;
				const SkinnedMesh::material& material = m->materials.at(key);

				//キー取得後変更前も保存
				loadTexture(material.shader_resource_views[0].Get());

				return;
			}
		}
	}
}

void MeshTexChange::update(float elapsedTime)
{

}

void MeshTexChange::OnGUI()
{
	for (int i = 0; i < texArray.size(); ++i)
	{
		ImGui::PushID(i);

		ImGui::Image(texArray.at(i)->getShaderResourceView().Get(), {32, 32});
		ImGui::SameLine();

		if (ImGui::Button("change texture") )changeMeshTex(i);
		ImGui::Separator();
		ImGui::PopID();
	}
}

void MeshTexChange::loadTexture(std::wstring filename)
{
	//変更可能な画像の登録
	texArray.push_back(
		std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), filename.c_str())
	);
}

void MeshTexChange::loadTexture(ID3D11ShaderResourceView* srv)
{
	texArray.push_back(
		std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), srv)
	);
}

void MeshTexChange::changeMeshTex(int textureNum)
{
	getObject()->getModel()->changeMaterial(key, texArray.at(textureNum)->getShaderResourceView());
}