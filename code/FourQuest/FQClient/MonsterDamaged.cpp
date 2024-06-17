#include "MonsterDamaged.h"
#include "Monster.h"

fq::client::MonsterDamaged::MonsterDamaged()
	:mDamagedDelay(100)
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
	if (mDamagedDelay > monster->GetDamagedDelay())
	{
		mDamagedDelay = 0;
		monster->SetHP(monster->GetHP() - monster->GetDamaged());
	}

	if (monster->GetHP() <= 0)
	{
		animator.SetParameterTrigger("OnDie");
		return;
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
	Monster* monster = animator.GetComponent<Monster>();
	mDamagedDelay += dt;
	if (monster->GetIsDamaged() && mDamagedDelay > monster->GetDamagedDelay() && monster->GetLastAttackTime() < 0.1f)
	{
		animator.SetParameterTrigger("OnDamaged");
		return;
	}
}

void fq::client::MonsterDamaged::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
}
