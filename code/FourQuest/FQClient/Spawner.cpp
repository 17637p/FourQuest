#include "Spawner.h"

std::shared_ptr<fq::game_module::Component> fq::client::Spawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpawner = std::dynamic_pointer_cast<Spawner>(clone);

	if (cloneSpawner == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<Spawner>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

