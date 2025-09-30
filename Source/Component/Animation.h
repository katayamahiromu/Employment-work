#pragma once
#include"Component.h"
#include"Graphics/SkinnedMesh.h"

class Animation : public Component
{
public:
	Animation(){}
	~Animation(){}

	// 名前取得
	const char* getName() const override { return "Animation"; }

	// 開始処理
	void prepare()override;

	// 更新処理
	void update(float elapsedTime);

	// GUI描画
	void OnGUI();

	//アニメーション再生
	void playAnimation(int index, bool loop, float blendRate = 0.2f);

	//アニメーション再生中か
	bool isPlayAnimation();

	float getAnimationTick() { return animation_tick; }
	void setAnimationTick(float time) { animation_tick = time; }
	int getAnimationIndex() { return animation_index; }
	void setAnimationIndex(int index) { animation_index = index; }
	animation::keyframe getKeyframe() { return keyframe; }

private:
	//アニメーション番号
	int animation_index = 1;
	animation::keyframe keyframe;

	//アニメーション再生時間
	float animation_tick = 0;

	//true...アニメーションがループした
	bool animationLoopFlag = false;

	//true...アニメーションが終了した
	bool animationEndFlag = false;

	//アニメーションブレンドの経過の合計時間
	float animationBlendTime = 0.0f;

	//アニメーションブレンドの時間
	float animationBlendSeconds = 0.0f;
};