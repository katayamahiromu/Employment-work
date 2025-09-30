#pragma once
#include"UI.h"
#include<list>
#include<set>

class UIManager
{
public:
	UIManager();
	~UIManager();

	void update(float elapsedTime);
	void render(ID3D11DeviceContext*dc);

	void registerUi(UI* ui) { UIArray.emplace_back(ui); }
	void remove(UI* ui) { removeArray.insert(ui); }
	void clear();
private:
	std::list<UI*>UIArray;
	std::set<UI*>removeArray;
};