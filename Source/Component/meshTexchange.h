#pragma once
#include"Component.h"
#include"Graphics/Sprite.h"

class MeshTexChange : public Component
{
public:
	MeshTexChange(const char*materialName);
	~MeshTexChange();

	// 名前取得
	const char* getName() const override { return "Mesh Texture change"; }

	// 開始処理
	void prepare()override;

	// 更新処理
	void update(float elapsedTime)override;

	// GUI描画
	void OnGUI()override;

	//変更可能な画像を連番登録
	void loadTexture(std::wstring filename);
	void loadTexture(ID3D11ShaderResourceView* srv);

	//登録された番号に変更
	void changeMeshTex(int textureNum);
private:
	//どのメッシュに対してのテクスチャかのキー
	uint64_t key = -1;
	const char* materialName;
	std::vector<std::unique_ptr<Sprite>>texArray;
};