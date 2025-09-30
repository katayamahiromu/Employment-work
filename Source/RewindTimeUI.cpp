#include"RewindTimeUI.h"
#include"DeviceManager.h"
#include"math/Mathf.h"

RewindTimeUI::RewindTimeUI(TimeLapse& tl):tl(tl)
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	baseLine = std::make_unique<Sprite>(device,nullptr);
	gage = std::make_unique<Sprite>(device, nullptr);
}

RewindTimeUI::~RewindTimeUI()
{

}

void RewindTimeUI::update(float elapsedTime)
{

	//バッファーの占有率によってゲージ幅を変更
	gageWidth = 292 * (1.0f - tl.occupancyRate() / 100.0f);

	// ゲージの幅に応じて色を変更
	if (gageWidth >= 292 * TWO_THIRDS) {
		color = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑
	}
	else if (gageWidth >= 292 * ONE_THIRDS) {
		color = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄色
	}
	else {
		color = { 1.0f, 0.0f, 1.0f, 1.0f }; // マゼンタ
	}
}

void RewindTimeUI::render(ID3D11DeviceContext* dc)
{
	baseLine->render(dc, { 10.0f, 10.0f, 300.0f, 50.f }, { 0.2f,0.2f,0.2f,0.8f });
	gage->render(dc, { 14.0f,14.0,gageWidth,42.0f }, color);
}