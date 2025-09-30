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
		//�u�����h���v�Z
		float blendRate = 1.0f;
		if (animationBlendSeconds > animationBlendTime)
		{
			animationBlendTime += elapsedTime;
			if (animationBlendTime >= animationBlendSeconds)
			{
				animationBlendTime = animationBlendSeconds;
			}
			blendRate = animationBlendTime / animationBlendSeconds;

			//�񎟊֐��I�ɕ⊮����
			blendRate *= blendRate;
		}

		//�u�����h�⊮
		if (blendRate < 1.0f)
		{
			//�o�͌��ʗp�̃L�[�t���[��
			animation::keyframe outKeyframe;
			//�J�n�A�j���[�V�����ƏI���A�j���[�V����
			const animation::keyframe* keyframes[2] =
			{
				&keyframe,
				&model->animation_clips.at(animation_index).sequence.at(0)
			};

			//�u�����h�A�j���[�V����
			model->blend_animations(keyframes, blendRate, outKeyframe);

			//�u�����h�⊮�����L�[�t���[���ōX�V
			model->update_animation(outKeyframe);

			keyframe = outKeyframe;
			model->setKeyframe(&keyframe);
		}
		//�ʏ�̏���
		else
		{
			//���f���A�j���[�V�����X�V
			int frame_index{ 0 };
			animation& animation{ model->animation_clips.at(animation_index) };

			//�A�j���[�V�������I�����Ă��Ȃ�������t���[���̌v�Z���s��
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