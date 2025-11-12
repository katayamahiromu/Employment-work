#pragma once
#include"Graphics/Sprite.h"
#include<memory>

class Button
{
public:
	Button(const wchar_t* filename);
	~Button();

	void draw();
	
	void loadReplaceSprite(const wchar_t* filename);

	void setPos(DirectX::XMFLOAT2 p) { pos = p; }
	void setSize(DirectX::XMFLOAT2 s) { size = s; }
	void changeReplaceFlag() { replaceFlag = !replaceFlag; }
	void replaceFlagOn() { replaceFlag = true; }
	void replaceFlagOff() { replaceFlag = false; }
private:
	DirectX::XMFLOAT2 pos = { 0.0f,0.0f };
	DirectX::XMFLOAT2 size = { 0.0f,0.0f };
	bool replaceFlag = false;
	std::unique_ptr<Sprite>sprite;

	//ïœçXâ¬î\Ç»ï®ÇàÍÇ¬
	std::unique_ptr<Sprite>replace;
};