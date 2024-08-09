#include "ThreadPool.h"
#include <windows.h>
#include <cassert>

fq::game_module::ThreadPool* fq::game_module::ThreadPool::GetInstance()
{
	assert(mInstance);

	return mInstance;
}

void fq::game_module::ThreadPool::Initialize()
{
	assert(mInstance == nullptr);
	//mInstance = new ThreadPool(std::thread::hardware_concurrency());
	mInstance = new ThreadPool(4);
}

void fq::game_module::ThreadPool::Finalize()
{
	assert(mInstance);

	delete mInstance;
}

fq::game_module::ThreadPool::ThreadPool(size_t numThreads)
	:mNumThreads(numThreads)
	,mIsWaitThreads(numThreads)
	,mbIsStopAll(false)
{
	mWorkerThreads.reserve(mNumThreads);

	for (size_t i = 0; i < numThreads; ++i)	
	{
		mIsWaitThreads[i] = false;
		mWorkerThreads.emplace_back([this, i]() {this->workerThread(i); });
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

void fq::game_module::ThreadPool::workerThread(size_t index)
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mJobMutex);
		mJobCV.wait(lock, [this, index]() 
			{
				mIsWaitThreads[index] = true;
				return !this->mJobs.empty() || mbIsStopAll; 
			});

		mIsWaitThreads[index] = false;

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

void fq::game_module::ThreadPool::Wait()
{
	bool allWait = false;
	
	while (!allWait)
	{
		allWait = true;

		for (int i = 0; i < mIsWaitThreads.size(); ++i)
		{
			if (!mIsWaitThreads[i])
			{
				allWait = false;
			}
		}
	}

	return;
}
