#define  NOMINMAX
#include "MonsterSpawner.h"

fq::client::MonsterSpawner::MonsterSpawner()
	:mSpawnCoolTime(0.f)
	, mSpawnElapsedTime(0.f)
{}

fq::client::MonsterSpawner::~MonsterSpawner()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MonsterSpawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpawner = std::dynamic_pointer_cast<MonsterSpawner>(clone);

	if (cloneSpawner == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawner>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSpawner = *this;
	}

	return cloneSpawner;
}

void fq::client::MonsterSpawner::OnUpdate(float dt)
{
	mSpawnElapsedTime = std::max(mSpawnElapsedTime - dt, 0.f);

	// ���� ����
	if (mSpawnElapsedTime == 0.f)
	{
		mSpawnElapsedTime = mSpawnCoolTime;

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMonster);
		auto& monster = *(instance.begin());

		auto spawnerT = GetComponent<game_module::Transform>();
		auto monsterT = monster->GetComponent<game_module::Transform>();

		monsterT->SetLocalPosition(spawnerT->GetLocalPosition());

		GetScene()->AddGameObject(monster);
	}

}
