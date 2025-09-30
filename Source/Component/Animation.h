#pragma once
#include"Component.h"
#include"Graphics/SkinnedMesh.h"

class Animation : public Component
{
public:
	Animation(){}
	~Animation(){}

	// ���O�擾
	const char* getName() const override { return "Animation"; }

	// �J�n����
	void prepare()override;

	// �X�V����
	void update(float elapsedTime);

	// GUI�`��
	void OnGUI();

	//�A�j���[�V�����Đ�
	void playAnimation(int index, bool loop, float blendRate = 0.2f);

	//�A�j���[�V�����Đ�����
	bool isPlayAnimation();

	float getAnimationTick() { return animation_tick; }
	void setAnimationTick(float time) { animation_tick = time; }
	int getAnimationIndex() { return animation_index; }
	void setAnimationIndex(int index) { animation_index = index; }
	animation::keyframe getKeyframe() { return keyframe; }

private:
	//�A�j���[�V�����ԍ�
	int animation_index = 1;
	animation::keyframe keyframe;

	//�A�j���[�V�����Đ�����
	float animation_tick = 0;

	//true...�A�j���[�V���������[�v����
	bool animationLoopFlag = false;

	//true...�A�j���[�V�������I������
	bool animationEndFlag = false;

	//�A�j���[�V�����u�����h�̌o�߂̍��v����
	float animationBlendTime = 0.0f;

	//�A�j���[�V�����u�����h�̎���
	float animationBlendSeconds = 0.0f;
};