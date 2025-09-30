#include "misc.h"
#include "SkinnedMesh.h"
#include <sstream>
#include <functional>
#include "shader.h"
#include <filesystem>
#include "texture.h"
#include <fstream>
#include<stdio.h>
#include"Buffer.h"

using namespace DirectX;

inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}

struct bone_influence
{
	uint32_t bone_index;
	float bone_weight;
};
using bone_influences_per_control_point = std::vector<bone_influence>;

void fetch_bone_influences(const FbxMesh* fbx_mesh,
	std::vector<bone_influences_per_control_point>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin
		{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

		const int cluster_count{ fbx_skin->GetClusterCount() };
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight
				{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
				bone_influence.bone_weight = static_cast<float>(control_point_weight);
			}
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");

	// cerealファイルが存在する場合はデシリアライズ処理
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips);
	}
	else
	{

		// FBX マネージャーと FBX シーンを作成
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

		// FBX インポーターを作成しファイルを読み込む
		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		// 読み込んだファイルのデータを FBX シーンに展開
		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		// true...ポリゴンをすべて三角形にする
		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate)
		{
			fbx_converter.Triangulate(fbx_scene, true, false);
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		// FBX シーンからデータをすべて取り出す
		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {

			scene::node& node{ scene_view.nodes.emplace_back() };
			node.attribute = fbx_node->GetNodeAttribute() ?
				fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
			node.name = fbx_node->GetName();
			node.unique_id = fbx_node->GetUniqueID();
			node.parent_index = scene_view.indexof(fbx_node->GetParent() ?
				fbx_node->GetParent()->GetUniqueID() : 0);

			for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
			{
				traverse(fbx_node->GetChild(child_index));
			}
		} };
		traverse(fbx_scene->GetRootNode());

		fetch_meshes(fbx_scene, meshes);

		fetch_materials(fbx_scene, materials);

		float sampling_rate = 0;
		fetch_animations(fbx_scene, animation_clips, sampling_rate);

		fbx_manager->Destroy();

		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serializetion(ofs);
		serializetion(scene_view, meshes, materials, animation_clips);

	}
	create_com_objects(device, fbx_filename);
}

/// <summary>
/// メッシュ情報の抽出（メッシュ情報の取り出し）
/// </summary>
/// <param name="fbx_scene">モデルのデータ</param>
/// <param name="meshes">抽出したメッシュ情報（出力用）</param>
void SkinnedMesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
	for (const scene::node& node : scene_view.nodes)
	{
		// メッシュでは無いノードはここで省く
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		// ノードからメッシュを取得
		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

		// メッシュ情報を取り出す
		mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);

		fetch_skeleton(fbx_mesh, mesh.bind_pose);

		std::vector<mesh::subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };

		// 利用されるすべてのマテリアルの数分のサブセットを作成
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}

		if (material_count > 0)
		{
			// ポリゴンの数から頂点の数を計算して、利用するサブセットのそれぞれのインデックスの数を計算
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index
				{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}

			// インデックスの数からサブセットの開始位置を計算
			uint32_t offset{ 0 };
			for (mesh::subset& subset : subsets)
			{
				subset.start_index_location = offset;
				offset += subset.index_count;
				subset.index_count = 0;
			}
		}

		// ポリゴンの数を取得
		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);

		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);

		// すべてのコントロールポイントを取得
		const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
		// ポリゴンの数だけ回す
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			const int material_index{ material_count > 0 ?
				fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			mesh::subset& subset{ subsets.at(material_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };

			// 三角形ポリゴンなので３回回す
			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				// 頂点データのインデックスを計算
				const int vertex_index{ polygon_index * 3 + position_in_polygon };

				// 頂点座標を取得
				vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				const bone_influences_per_control_point& influences_per_control_point
				{ bone_influences.at(polygon_vertex) };
				for (size_t influence_index = 0; influence_index < influences_per_control_point.size();
					++influence_index)
				{
					if (influence_index < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[influence_index] =
							influences_per_control_point.at(influence_index).bone_weight;
						vertex.bone_indices[influence_index] =
							influences_per_control_point.at(influence_index).bone_index;
					}
				}



				// 法線の取得
				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				//法線ベクトルの取得
				if (fbx_mesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
				}

				// テクスチャ座標の取得
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
						uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				/*for (const auto& v : mesh.vertices)
				{
					mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
					mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
					mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
					mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
					mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
					mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
				};*/

				mesh.vertices.at(vertex_index) = std::move(vertex);
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
			}
		}
	}
}

/// <summary>
/// マテリアル情報の抽出
/// </summary>
/// <param name="fbx_scene">モデルのデータ</param>
/// <param name="materials">抽出したマテリアル情報（出力用）</param>
void SkinnedMesh::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
	const size_t node_count{ scene_view.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		const scene::node& node{ scene_view.nodes.at(node_index) };
		const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

		const int material_count{ fbx_node->GetMaterialCount() };
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

			material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();

			FbxProperty fbx_property;

			// ディフューズの情報
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbx_property.IsValid())
			{
				// ディフューズ色
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				// テクスチャ情報（ディフューズ情報のみ）
				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[0] =
					fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}

			// スペキュラーの情報
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbx_property.IsValid())
			{
				// スペキュラー色
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;
			}

			// アンビエントの情報
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbx_property.IsValid())
			{
				// アンビエント色
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;
			}
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbx_property.IsValid())
			{
				const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
			}
			materials.emplace(material.unique_id, std::move(material));
		}
	}

	materials.emplace();
}

/// <summary>
/// バインドポーズの抽出
/// </summary>
/// <param name="fbx_mesh">モデルデータ</param>
/// <param name="bind_pose">抽出したバインドポーズ（オフセット行列）</param>
void SkinnedMesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
	const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
		const int cluster_count = skin->GetClusterCount();
		bind_pose.bones.resize(cluster_count);
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			FbxCluster* cluster = skin->GetCluster(cluster_index);

			skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
			bone.name = cluster->GetLink()->GetName();
			bone.unique_id = cluster->GetLink()->GetUniqueID();
			bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.node_index = scene_view.indexof(bone.unique_id);

			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			bone.offset_transform
				= to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
		}
	}
}

/// <summary>
/// アニメーションデータの抽出
/// </summary>
/// <param name="fbx_scene">モデルデータ</param>
/// <param name="animation_clips">抽出したアニメーションデータ</param>
/// <param name="sampling_rate">サンプリングレート（０がデフォルト）</param>
void SkinnedMesh::fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
	float sampling_rate)
{
	FbxArray<FbxString*> animation_stack_names;
	fbx_scene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count{ animation_stack_names.GetCount() };

	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		animation& animation_clip{ animation_clips.emplace_back() };
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

		FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
		fbx_scene->SetCurrentAnimationStack(animation_stack);

		const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = sampling_rate > 0 ?
			sampling_rate : static_cast<float>(one_second.GetFrameCount(time_mode));
		const FbxTime sampling_interval
		{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
		const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
		const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
		const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

			const size_t node_count{ scene_view.nodes.size() };
			keyframe.nodes.resize(node_count);
			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
				if (fbx_node)
				{
					animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
					//シーンのグローバル座標
					node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

					//シーンのローカル座標系
					const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
					node.local_transform = to_xmfloat4x4(local_transform);
					node.scaling = to_xmfloat3(local_transform.GetS());
					node.rotation = to_xmfloat4(local_transform.GetQ());
					node.translation = to_xmfloat3(local_transform.GetT());
				}
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}
}

/// <summary>
/// COM オブジェクトの生成
/// </summary>
void SkinnedMesh::create_com_objects(ID3D11Device* device, const char* fbx_filename)
{
	for (mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = mesh.vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
#if 0
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	ShaderManager::instance()->createVsFromCso(device, ".\\Shader\\SkinnedMeshVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\SkinnedMeshPS.cso", pixel_shader.ReleaseAndGetAddressOf());
	ShaderManager::instance()->createGsFromCso(device, ".\\Shader\\SkinnedMeshGS.cso", geometry_shader.ReleaseAndGetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin();
		iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < 2; ++texture_index)
		{
			if (iterator->second.texture_filenames[texture_index].size() > 0)
			{
				std::filesystem::path path(fbx_filename);
				path.replace_filename(iterator->second.texture_filenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				TextureManager::instance()->loadTextureFromFile(device, path.c_str(),
					iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else
			{
				TextureManager::instance()->makeDummyTexture(device,
					iterator->second.shader_resource_views[texture_index].GetAddressOf(),
					texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}
}

//モデルにアニメーションを追加する関数
//animation_faillname
bool SkinnedMesh::append_animation(const char* animation_finename, float sampling_rate)
{
	{
		//外部からアニメーションを読み込んで
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

		//インポートして
		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
		bool import_status{ false };
		import_status = fbx_importer->Initialize(animation_finename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		//アニメーションのボーン行列に対してアニメーションを付ける
		fetch_animations(fbx_scene, animation_clips, sampling_rate);

		fbx_manager->Destroy();

		return true;
	}
}

//アニメーションのブレンド補間
//keyframe[0] : 開始のフレーム [1]が終了のフレーム
void SkinnedMesh::blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe)
{
	size_t node_count{ keyframes[0]->nodes.size() };
	keyframe.nodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		DirectX::XMVECTOR S[2]
		{
			DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
			DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
		DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

		DirectX::XMVECTOR R[2]
		{
			DirectX::XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
			DirectX::XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		DirectX::XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

		DirectX::XMVECTOR T[2]
		{
			DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
			DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
	}
}


//アニメーション更新処理
void SkinnedMesh::update_animation(animation::keyframe& keyframe)
{
	size_t node_count{ keyframe.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
		XMMATRIX S{ XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
		XMMATRIX T{ XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

		int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
		XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };
		XMStoreFloat4x4(&node.global_transform, S * R * T * P);
	}
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">ワールド行列</param>
/// <param name="material_color">モデル全体の色</param>
/// <param name="keyframe">1フレームのモデルの全ボーンの姿勢行列</param>
void SkinnedMesh::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
	const animation::keyframe* keyframe)
{
	// メッシュごとに描画処理を行う
	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediate_context->IASetInputLayout(input_layout.Get());

		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
		immediate_context->GSSetShader(geometry_shader.Get(), nullptr, 0);

		constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			for (int bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
		}
		else
		{
			DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.default_global_transform) * DirectX::XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.bone_transforms[bone_index] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
			}
		}
		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };

			XMStoreFloat4(&data.material_color,
				XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			ID3D11ShaderResourceView* resorce[] =
			{
				material.shader_resource_views[0].Get(),
				material.shader_resource_views[1].Get(),
			};
			immediate_context->PSSetShaderResources(0, 2, resorce);

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}


void SkinnedMesh::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
	// メッシュごとに描画処理を行う
	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediate_context->IASetInputLayout(input_layout.Get());

		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
		immediate_context->GSSetShader(geometry_shader.Get(), nullptr, 0);

		constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			for (int bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
		}
		else
		{
			DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.default_global_transform) * DirectX::XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.bone_transforms[bone_index] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
			}
		}
		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };

			XMStoreFloat4(&data.material_color,
				XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			ID3D11ShaderResourceView* resorce[] =
			{
				material.shader_resource_views[0].Get(),
				material.shader_resource_views[1].Get(),
			};
			immediate_context->PSSetShaderResources(0, 2, resorce);

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}

int SkinnedMesh::findBoneIndex(const char* name)
{
	for (const SkinnedMesh::mesh& mesh : meshes)
	{
		for (const skeleton::bone& bone : mesh.bind_pose.bones)
		{
			if (std::strcmp(bone.name.c_str(), name) == 0)
				return static_cast<int>(bone.node_index);
		}
	}
	return -1;
}

//既にメッシュ番号が分かっている場合
int SkinnedMesh::findBoneIndex(const char* name, int meshIndex)
{
	for (const skeleton::bone& bone : meshes.at(meshIndex).bind_pose.bones)
	{
		if (std::strcmp(bone.name.c_str(), name) == 0)
			return static_cast<int>(bone.node_index);
	}
	return -1;
}

int SkinnedMesh::findMeshIndex(const char* name)
{
	//設定されている番号とメッシュの配列の番号が一致していないため範囲forは使えない
	for (int i = 0;i < meshes.size();++i)
	{
		if (std::strcmp(meshes.at(i).name.c_str(), name) == 0)
			return i;
	}
	return -1;
}

DirectX::XMFLOAT4X4 SkinnedMesh::boneMatrix(int meshIndex, int boneIndex, animation::keyframe& key)
{
	animation::keyframe::node& node = key.nodes.at(boneIndex);
	DirectX::XMMATRIX nodeMatrix = DirectX::XMLoadFloat4x4(&node.global_transform);
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, nodeMatrix);
	return mat;
}

DirectX::XMFLOAT3 SkinnedMesh::getBonePosition(int meshIndex, int boneIndex, animation::keyframe& key, const DirectX::XMFLOAT4X4& transform)
{
	animation::keyframe::node& node = key.nodes.at(boneIndex);
	DirectX::XMFLOAT3 pos = { node.global_transform._41,node.global_transform._42,node.global_transform._43 };
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&meshes.at(meshIndex).default_global_transform) * DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), M));
	return pos;
}