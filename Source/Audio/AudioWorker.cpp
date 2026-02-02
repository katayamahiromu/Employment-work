#include"AudioWorker.h"

AudioWorker::AudioWorker():terminate(false)
{
	worker = std::thread([this] {this->Run();});
}

AudioWorker::~AudioWorker()
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		terminate = true;
	}

	cv.notify_all();   // ロック外で通知

	if (worker.joinable())
	{
		worker.join();
	}
}

void AudioWorker::PushTask(Task task)
{
	std::lock_guard<std::mutex> lock(mtx);
	tasks.push(std::move(task));
	cv.notify_one();
}

void AudioWorker::Run()
{
	std::unique_lock<std::mutex>lock(mtx);
	while (!terminate)
	{
		cv.wait(lock, [&] { return !tasks.empty() || terminate; });
		if (terminate) break;

		Task task = std::move(tasks.front());
		tasks.pop();

		lock.unlock();
		task();
		lock.lock();
	}
}