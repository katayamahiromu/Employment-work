#pragma once
#include<functional>
#include<algorithm>

class  State
{
public:
	State(
		std::function<void()> enter,
		std::function<void(float)> execute,
		std::function<void()> exit)
		:enter(enter),execute(execute),exit(exit)
	{}
	~State() {};

	void requestEnter() { if (enter)enter(); }
	void requestExecute(float elapsedTime) { if (execute)execute(elapsedTime); }
	void requestExit() { if (exit)exit(); }
private:
	std::function<void()>enter;
	std::function<void(float)>execute;
	std::function<void()>exit;
};