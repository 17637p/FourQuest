#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <vector>

namespace fq::game_module
{
	class ThreadPool
	{
	public:
		static void Initialize();
		static ThreadPool* GetInstance();
		static void Finalize();

		template <class F, class... Args>
		std::future<std::invoke_result_t<F, Args...>> EnqueueJob(F&& f, Args&&... args);

		void Wait();

	private:
		ThreadPool(size_t numThreads);
		~ThreadPool();

		void workerThread(size_t index);

	private:
		inline static ThreadPool* mInstance = nullptr;

		size_t mNumThreads;
		std::vector<std::thread> mWorkerThreads;
		std::vector<std::atomic<bool>> mIsWaitThreads;
		std::queue<std::function<void()>> mJobs;
		std::condition_variable mJobCV;
		std::mutex mJobMutex;
		bool mbIsStopAll;
	};

	template<class F, class ...Args>
	inline std::future<std::invoke_result_t<F, Args...>> ThreadPool::EnqueueJob(F&& f, Args && ...args)
	{
		if (mbIsStopAll)
		{
			throw std::runtime_error("ThreadPool »ç¿ë ÁßÁöµÊ");
		}

		using return_type = typename std::invoke_result_t<F, Args...>;

		auto job = std::make_shared<std::packaged_task<return_type()>>
			(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> job_result_future = job->get_future();
		{
			std::lock_guard<std::mutex> lock(mJobMutex);
			mJobs.push([job]() {(*job)(); });
		}
		mJobCV.notify_one();

		return job_result_future;
	}
}