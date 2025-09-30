#pragma once
#include"Graphics/Sprite.h"
#include"Scene.h"
#include<memory>
#include<thread>

//ローディングシーン
class LoadingScene : public Scene
{
public:
	LoadingScene(Scene*nextScene):nextScene(nextScene){}
	~LoadingScene()override {};

	//初期化処理
	void initialize()override;

	//終了化
	void finalize()override;

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render()override;
private:
	//べつのスレッドとして起動させる関数
	static void LoadingThread(LoadingScene* scene);
private:
	//ローディング画面
	std::unique_ptr<Sprite>loadingImage;

	//回転角度
	float angle = 0.0f;

	//次のシーン
	Scene* nextScene = nullptr;

	//マルチ用スレッド
	std::thread* thread = nullptr;
};