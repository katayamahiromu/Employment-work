#pragma once

#include"Scene.h"

//シーン管理
class SceneManager
{
private:
	SceneManager(){}
	~SceneManager() { clear(); }
public:
	static SceneManager* instance()
	{
		static SceneManager inst;
		return &inst;
	}

	//更新処理
	void update(float elapsedTime);

	//描画処理
	void render();

	//現在のシーンをクリア
	void clear();

	//シーンの切り替え（外部で生成したシーンを次のシーンとして設定）
	void changeScene(Scene* scene);
private:
	//現在のシーン
	Scene* currentScene = nullptr;

	//次のシーン
	Scene* nextScene = nullptr;
};