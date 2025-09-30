#pragma once

#include"object.h"

//コンポーネント
class Component
{
public:
	Component() {};
	virtual ~Component() {};

	// 名前取得
	virtual const char* getName() const = 0;

	// 開始処理
	virtual void prepare() {}

	// 更新処理
	virtual void update(float elapsedTime) {}

	// GUI描画
	virtual void OnGUI() {}

	// アクター設定
	void setObject(std::shared_ptr<Object> object) { this->object = object; }

	// アクター取得
	std::shared_ptr<Object> getObject() { return object.lock(); }
private:
	std::weak_ptr<Object>object;
};