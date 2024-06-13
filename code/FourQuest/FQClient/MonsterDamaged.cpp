#include "MonsterDamaged.h"
#include "Monster.h"

fq::client::MonsterDamaged::MonsterDamaged()
{

}

fq::client::MonsterDamaged::~MonsterDamaged()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterDamaged::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterDamaged>(*this);
}

void fq::client::MonsterDamaged::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	Monster* monster = animator.GetComponent<Monster>();
	monster->SetHP(monster->GetHP() - monster->GetDamaged());

	if (monster->GetHP() <= 0)
	{
		animator.SetParameterTrigger("OnDie");
	}
	else
	{
		monster->SetIsDamaged(false);

		animator.SetParameterTrigger("OnIdle");
		monster->SetTarget(monster->GetLastAttacker());
	}
}

void fq::client::MonsterDamaged::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::MonsterDamaged::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}
