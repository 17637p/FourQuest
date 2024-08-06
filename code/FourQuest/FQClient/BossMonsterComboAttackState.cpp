#include "BossMonsterComboAttackState.h"

#include "BossMonster.h"

fq::client::BossMonsterComboAttackState::BossMonsterComboAttackState()
	:mAttackElapsedTime(0.f)
	,mEmitAttackTime(0.f)
{}

fq::client::BossMonsterComboAttackState::~BossMonsterComboAttackState()
{}

void fq::client::BossMonsterComboAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	animator.GetComponent<BossMonster>()->ReboundComboAttack();
}

void fq::client::BossMonsterComboAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mAttackElapsedTime != mEmitAttackTime)
	{
		mAttackElapsedTime += dt;

		if (mAttackElapsedTime >= mEmitAttackTime)
		{
			animator.GetComponent<BossMonster>()->EmitComboAttack();
			mAttackElapsedTime = mEmitAttackTime;
		}
	}

}

void fq::client::BossMonsterComboAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterComboAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterComboAttackState>(*this);
}
