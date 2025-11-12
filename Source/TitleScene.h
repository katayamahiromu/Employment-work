#pragma once
#include"Button.h"
#include"Scene.h"
#include"Component/object.h"
#include"Audio/AudioManager.h"
#include"Graphics/Skymap.h"
#include"Component/Animation.h"
#include"Component/PlayerController.h"
#include<string>
#include<memory>

#include"Graphics/Specturm.h"

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
	void updateSelector(float elapsedTime);
	enum Select
	{
		Start,
		Exit,
	};
	int select = 0;
	float paddingTime = 0;

	//入力停止時間
	const float freezeTime = 0.3f;
private:
	//3D関係
	std::unique_ptr<ObjectManager>objManager;
	std::shared_ptr<Object>model;
	std::shared_ptr<Animation>animation;
	
	//動き
	std::shared_ptr<PlayerController>controller;

	//2D関係
	std::unique_ptr<Sprite>titleImage;
	std::unique_ptr<Button>rogo;

	std::vector<Button*>choices;

	std::unique_ptr<Audio>sample;
	std::unique_ptr<Audio>sampleRevers;
	std::unique_ptr<Spectrum>spectrum;

	float rate = 1.0f;
};