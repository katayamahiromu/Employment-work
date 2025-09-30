#include"SceneManager.h"

//更新処理
void SceneManager::update(float elapsedTime)
{
	//次のシーンが設定されているか
	if (nextScene != nullptr)
	{
		//現在のシーンを破棄
		clear();

		//次のシーンを現在のシーンとして設定
		currentScene = nextScene;

		//次のシーンをnullptrを代入
		nextScene = nullptr;

		//シーンを初期化
		if (currentScene->isReady() == false)
		{
			currentScene->initialize();
		}
	}

	if (currentScene != nullptr)
	{
		currentScene->update(elapsedTime);
	}
}

//描画処理
void SceneManager::render()
{
	if (currentScene != nullptr)
	{
		currentScene->render();
	}
}

//現在のシーンをクリア
void SceneManager::clear()
{
	if (currentScene != nullptr)
	{
		currentScene->finalize();
		delete currentScene;
		currentScene = nullptr;
	}
}

//シーンの切り替え（外部で生成したシーンを次のシーンとして設定）
void SceneManager::changeScene(Scene* scene)
{
	//次のシーンとして設定
	nextScene = scene;
}