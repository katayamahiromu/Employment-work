#include"StateMachine.h"

StateMachine::StateMachine()
{

}

StateMachine::~StateMachine()
{
	for (auto& pair : statePool)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	statePool.clear();
}

void StateMachine::update(float elapsedTime)
{
	//現在のステートを実行
	if (currentState != nullptr)currentState->requestExecute(elapsedTime);
}

void StateMachine::changeState(int num)
{
	if (currentState)currentState->requestExit();

	auto it = statePool.find(num);
	if (it != statePool.end())
	{
		currentState = it->second;
		currentType = it->first;
		currentState->requestEnter();
	}
}

void StateMachine::registerState(int num, State* state)
{
	if (!state)return;
	statePool.insert({ num,state });
}