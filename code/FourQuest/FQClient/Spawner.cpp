#include "Spawner.h"

std::shared_ptr<fq::game_module::Component> fq::client::Spawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpawner = std::dynamic_pointer_cast<Spawner>(clone);

	if (cloneSpawner == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<Spawner>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSpawner = *this;
	}

	return cloneSpawner;
}

fq::client::Spawner::Spawner()
	:mID(0)
{

}

fq::client::Spawner::~Spawner()
{

}

void fq::client::Spawner::OnStart()
{

}

void fq::client::Spawner::OnUpdate(float dt)
{

}

