#pragma once
#include"Graphics/Sprite.h"
#include"Scene.h"
#include"../RingBuffer.h"
#include"Component/object.h"
#include"CameraController.h"
#include<string>
#include<memory>

//タイトルシーン
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//初期化
	void initialize() override;

	//終了化
	void finalize()override;

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render()override;

	void Gui();
private:
	//タイトル画像
	std::unique_ptr<ObjectManager>objManager;
	std::unique_ptr<CameraController>cmrController;
	std::unique_ptr<Sprite>titleImage;

	int count = 5;
	RingBuffer<std::string, 5>test;
	char text[32];
};