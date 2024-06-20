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

	if (cloneSpawner == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawner>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSpawner = *this;
	}

	return cloneSpawner;
}

void fq::client::MonsterSpawner::OnUpdate(float dt)
{
	mSpawnElapsedTime = std::max(mSpawnElapsedTime - dt, 0.f);

	// 몬스터 생성
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
