#pragma once
#include"Graphics/Sprite.h"
#include"Scene.h"
#include"../RingBuffer.h"
#include"Component/object.h"
#include"CameraController.h"
#include"Audio/AudioManager.h"
#include<string>
#include<memory>

//�^�C�g���V�[��
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//������
	void initialize() override;

	//�I����
	void finalize()override;

	//�X�V����
	void update(float elapsedTime)override;

	//�`�揈��
	void render()override;

	void Gui();
private:
	//�^�C�g���摜
	std::unique_ptr<ObjectManager>objManager;
	std::unique_ptr<CameraController>cmrController;
	std::unique_ptr<Sprite>titleImage;

	std::unique_ptr<Audio>sample;
	std::unique_ptr<Audio>sampleRevers;

	bool flag = true;
	int count = 5;
	RingBuffer<std::string, 5>test;
	char text[32];
};