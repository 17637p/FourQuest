#include "MonsterSpawnerDetector.h"

#include "../FQGameModule/GameModule.h"
#include "MonsterSpawner.h"

std::shared_ptr<fq::game_module::Component> fq::client::MonsterSpawnerDetector::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MonsterSpawnerDetector> cloneMonster = std::dynamic_pointer_cast<MonsterSpawnerDetector>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawnerDetector>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MonsterSpawnerDetector::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player
		|| collision.other->GetTag() == game_module::ETag::Dash
		|| collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
	{
		auto parent = GetGameObject()->GetParent();
		if (parent)
		{
			auto monsterSpawner = parent->GetComponent<MonsterSpawner>();
			monsterSpawner->DetectPlayer(collision.other);
			// 활성화 이후에는 파괴합니다
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}
