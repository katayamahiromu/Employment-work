#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<functional>

class AudioWorker
{
public:
	using Task = std::function<void()>;

	AudioWorker();
	~AudioWorker();

	void PushTask(Task task);

	int TaskCount() { return static_cast<int>(tasks.size()); }
private:
	void Run();
private:
	std::thread worker;
	std::mutex mtx;
	std::condition_variable cv;

	std::queue<Task> tasks;
	std::atomic<bool> terminate;
};