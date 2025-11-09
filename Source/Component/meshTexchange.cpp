#include"meshTexchange.h"
#include"imgui.h"
#include"Graphics/Texture.h"
#include"DeviceManager.h"
#include"../Utils/convertStrings.h"

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
				 
				//モデルからのパス
				const std::wstring name = StringToWString(material.texture_filenames->c_str());
				const std::wstring directory = extractionDirectory(m->getName());

				const std::wstring file = directory + name;
				loadTexture(file.c_str());

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
	
	int id = 0;
	for (auto& tex:texArray)
	{
		ImGui::PushID(id);

		ImGui::Image(tex.second->getShaderResourceView().Get(), {32, 32});
		ImGui::SameLine();

		if (ImGui::Button("change texture") )changeMeshTex(tex.first);
		ImGui::Separator();
		ImGui::PopID();

		id++;
	}
}

void MeshTexChange::loadTexture(const wchar_t* filename)
{
	//変更可能な画像の登録
	texArray.insert({
		filename,
		std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), filename)
		});
}

void MeshTexChange::changeMeshTex(const wchar_t* name)
{
	getObject()->getModel()->changeMaterial(key, texArray.at(name)->getShaderResourceView());
}

const std::wstring MeshTexChange::extractionDirectory(const char* file)
{
	std::string str = file;

	//ディレクトリ部分を抽出
	size_t pos = str.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);
	}

	//wString型に変換
	const std::wstring wstr = StringToWString(str);
	return wstr;
}