#include "MonsterIdle.h"
#include "Monster.h"

fq::client::MonsterIdle::MonsterIdle()
{
}

fq::client::MonsterIdle::~MonsterIdle()
{
}

void fq::client::MonsterIdle::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
}

void fq::client::MonsterIdle::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{
	Monster* monster = animator.GetComponent<Monster>();

	if (monster->GetTarget() == nullptr)
	{
		animator.SetParameterTrigger("OnFind");
	}
	else
	{
		if (monster->GetIsDamaged())
		{
			animator.SetParameterTrigger("OnDamaged");
		}
		else
		{
			float targetDist = monster->CalculateDistanceTarget(*monster->GetTarget());

			if (targetDist < monster->GetTargetAttackRange())
			{
				animator.SetParameterTrigger("OnAttackPreparation");

			}
			else
			{
				animator.SetParameterTrigger("OnChase");
			}
		}
	}
}

void fq::client::MonsterIdle::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterIdle::Clone()
{
	return  fq::game_module::ObjectPool::GetInstance()->Assign<MonsterIdle>(*this);
}
