#include "MeleeMonsterAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

fq::client::MeleeMonsterAttackState::MeleeMonsterAttackState()
	:mAttackTiming(0.f)
	,mElapsedTime(0.f)
{}

fq::client::MeleeMonsterAttackState::~MeleeMonsterAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterAttackState>(*this);
}

void fq::client::MeleeMonsterAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;

	// ���� ��� �ߵ�
	if (mAttackTiming == mElapsedTime)
	{
		animator.GetComponent<MeleeMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;
	}
}

void fq::client::MeleeMonsterAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
		return;

	mElapsedTime += dt;

	// ���� �ߵ� 
	if (mElapsedTime >= mAttackTiming)
	{
		animator.GetComponent<MeleeMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;
	}
}
