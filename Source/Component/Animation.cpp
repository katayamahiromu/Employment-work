#include"Animation.h"
#include"imgui.h"

void Animation::OnGUI()
{
	ImGui::InputFloat("animation_tick", &animation_tick);
	ImGui::InputInt("animation_index", &animation_index);
}

void Animation::prepare() 
{

}

void Animation::update(float elapsedTime)
{

	SkinnedMesh* model = getObject()->getModel();

	if (animation_index >= 0)
	{
		//ブレンド率計算
		float blendRate = 1.0f;
		if (animationBlendSeconds > animationBlendTime)
		{
			animationBlendTime += elapsedTime;
			if (animationBlendTime >= animationBlendSeconds)
			{
				animationBlendTime = animationBlendSeconds;
			}
			blendRate = animationBlendTime / animationBlendSeconds;

			//二次関数的に補完する
			blendRate *= blendRate;
		}

		//ブレンド補完
		if (blendRate < 1.0f)
		{
			//出力結果用のキーフレーム
			animation::keyframe outKeyframe;
			//開始アニメーションと終了アニメーション
			const animation::keyframe* keyframes[2] =
			{
				&keyframe,
				&model->animation_clips.at(animation_index).sequence.at(0)
			};

			//ブレンドアニメーション
			model->blend_animations(keyframes, blendRate, outKeyframe);

			//ブレンド補完したキーフレームで更新
			model->update_animation(outKeyframe);

			keyframe = outKeyframe;
			model->setKeyframe(&keyframe);
		}
		//通常の処理
		else
		{
			//モデルアニメーション更新
			int frame_index{ 0 };
			animation& animation{ model->animation_clips.at(animation_index) };

			//アニメーションが終了していなかったらフレームの計算を行う
			if (animationEndFlag == false)
				frame_index = static_cast<int>(animation_tick * animation.sampling_rate);

			if (frame_index > animation.sequence.size() - 1)
			{
				frame_index = 0;
				animation_tick = 0;
				if (animationLoopFlag == false)
				{
					animationEndFlag = true;
					animation_index = -1;
				}
			}
			else
			{
				if (animationEndFlag == false)
					animation_tick += elapsedTime;
			}
			//keyframe = animation.sequence.at(frame_index);
			keyframe = animation.sequence.at(frame_index);
			model->setKeyframe(&keyframe);
			model->update_animation(keyframe);
		}
	}
}

void Animation::playAnimation(int index, bool loop, float blendRate)
{
	animation_index = index;
	animation_tick = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0;
	animationBlendSeconds = blendRate;
}

bool Animation::isPlayAnimation()
{
	SkinnedMesh* model = getObject()->getModel();
	if (animation_index < 0)return false;
	if (animation_index >= model->animation_clips.size())return false;
	return true;
}