#pragma once
#include"State.h"
#include<unordered_map>

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void update(float elapsedTime);

	void changeState(int num);

	void registerState(int num,State* state);

	int getCurrentType() { return currentType; }
private:
	//���݂̃X�e�[�g
	State* currentState = nullptr;

	//���݂̃X�e�[�g�̃i���o�[
	int currentType = -1;

	std::unordered_map<int, State*>statePool;
};
