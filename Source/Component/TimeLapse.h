#pragma once
#include"../Utils/RingBuffer.h"
#include"Component/Component.h"
#include"Component/Animation.h"

struct recodeInfo
{
	//アニメーション番号
	int animationIndex;
	//アニメーション再生時間
	float animationTick;
	DirectX::XMFLOAT4X4 Transform;
	//int actionType;
};

class TimeLapse : public Component
{
public:
	TimeLapse();
	~TimeLapse();

	// 名前取得
	const char* getName() const override { return "TimeLapse"; }

	// 開始処理
	void prepare()override;

	// 更新処理
	void update(float elapsedTime);

	// GUI描画
	void OnGUI();

	//記録
	void setRecordInfo();

	//記録の取り出し
	void outputRecordInfo();

	//バッファーの使用率
	float occupancyRate(){return static_cast<float>(recordArray.available()) / recordMax * 100;}
private:
	//後ろから順にアクセスする
	recodeInfo getRecord() { return recordArray.frontAndPop(); }

	bool recordEmpty() { return recordArray.isEmpty(); }
private:
	static constexpr int recordMax = 144 * 3;
	int subscript = 0;
	RingBuffer<recodeInfo, recordMax>recordArray;
	std::shared_ptr<Animation>animation;
	bool isRecord = true;
};