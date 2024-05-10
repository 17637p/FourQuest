#include "ObjectPool.h"

fq::game_module::ObjectPool* fq::game_module::ObjectPool::GetInstance()
{
	if (ObjectPool::mInstance == nullptr)
	{
		ObjectPool::mInstance = new ObjectPool();
	}

	return ObjectPool::mInstance;
}

void fq::game_module::ObjectPool::Finalize()
{
	if (mInstance == nullptr) return;

	for (auto& pool : mInstance->mPools)
	{
		pool.second->Clear();
	}

	mInstance->mPools.clear();

	delete mInstance;
}

