#include "ThreadPool.h"

#include <cassert>

fq::game_module::ThreadPool* fq::game_module::ThreadPool::GetInstance()
{
	assert(mInstance);

	return mInstance;
}

void fq::game_module::ThreadPool::Initialize()
{
	assert(mInstance == nullptr);
	mInstance = new ThreadPool(std::thread::hardware_concurrency());
}

void fq::game_module::ThreadPool::Finalize()
{
	assert(mInstance);

	delete mInstance;
}

fq::game_module::ThreadPool::ThreadPool(size_t numThreads)
	:mNumThreads(numThreads)
	,mbIsStopAll(false)
{
	mWorkerThreads.reserve(mNumThreads);

	for (size_t i = 0; i < numThreads; ++i)
	{
		mWorkerThreads.emplace_back([this]() {this->workerThread(); });
	}
}

fq::game_module::ThreadPool::~ThreadPool()
{
	mbIsStopAll = true;
	// 모든 쓰레드를 깨움
	mJobCV.notify_all();

	for (auto& t : mWorkerThreads)
	{
		t.join();
	}
}

void fq::game_module::ThreadPool::workerThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mJobMutex);
		mJobCV.wait(lock, [this]() {return !this->mJobs.empty() || mbIsStopAll; });

		if (mbIsStopAll && this->mJobs.empty())
		{
			return;
		}

		std::function<void()> job = std::move(mJobs.front());
		mJobs.pop();
		lock.unlock();

		job();
	}
}
