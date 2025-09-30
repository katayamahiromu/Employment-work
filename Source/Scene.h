#pragma once

//シーン
class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}

	//初期化処理
	virtual void initialize() = 0;

	//終了化
	virtual void finalize() = 0;

	//更新処理
	virtual void update(float elaspedTime)=0;

	//描画処理
	virtual void render() = 0;

	//true...準備完了
	bool isReady()const { return readyFlag; }

	//準備完了設定
	void setReady() { readyFlag = true; }

private:
	bool readyFlag = false;
};