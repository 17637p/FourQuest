#include "MonsterGroup.h"

#include "PlantMonster.h"
#include "MeleeMonster.h"

fq::client::MonsterGroup* fq::client::MonsterGroup::GetMonsterGroup(fq::game_module::GameObject* monster)
{
	auto parent = monster->GetParent();

	if (parent == nullptr)
	{
		return nullptr;
	}

	return parent->GetComponent<MonsterGroup>();
}

fq::client::MonsterGroup::MonsterGroup()
{

}

fq::client::MonsterGroup::~MonsterGroup()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MonsterGroup::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMonsterGroup = std::dynamic_pointer_cast<MonsterGroup>(clone);

	if (cloneMonsterGroup == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneMonsterGroup = game_module::ObjectPool::GetInstance()->Assign<MonsterGroup>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonsterGroup = *this;
	}

	return cloneMonsterGroup;
}

void fq::client::MonsterGroup::OnStart()
{
	// 그룹을 생성합니다 
	for (auto child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<MeleeMonster>()
			|| child->HasComponent<PlantMonster>())
		{
			mMonsters.push_back(child->shared_from_this());
		}
	}
}

void fq::client::MonsterGroup::OnUpdate(float dt)
{

}

void fq::client::MonsterGroup::Register(fq::game_module::GameObject* monster)
{
	mMonsters.push_back(monster->shared_from_this());
}

void fq::client::MonsterGroup::AnnounceFindedTarget(fq::game_module::GameObject* target)
{
	for (const auto& monster : mMonsters)
	{
		// Melee
		if (monster->HasComponent<MeleeMonster>() && !monster->IsDestroyed())
		{
			monster->GetComponent<MeleeMonster>()->SetTarget(target);
		}
	}
}

fq::game_module::GameObject* fq::client::MonsterGroup::GetTarget() const
{
	return mTarget.get();
}

