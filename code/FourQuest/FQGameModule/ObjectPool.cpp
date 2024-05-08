#include "ObjectPool.h"

fq::game_module::ObjectPool* fq::game_module::ObjectPool::GetInstance()
{
	if (ObjectPool::mPool == nullptr)
	{
		ObjectPool::mPool = new ObjectPool();
	}

	return ObjectPool::mPool;
}

fq::game_module::ObjectPool::~ObjectPool()
{

}

fq::game_module::ObjectPool::ObjectPool()
{

}

