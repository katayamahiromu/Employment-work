#pragma once

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
	void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4 cameraPos);

	//オブジェクトの全破棄
	void clear();

	//オブジェクト検索
	Object* searchObject(const char* name);

	//ラインサイズ設定
	void setLineSize(float size) { lineSize = size; }

	//ラインカラー設定
	void setlineColor(DirectX::XMFLOAT3& color) { lineColor = color; }
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