#include "BossMonsterComboAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "BossMonster.h"

fq::client::BossMonsterComboAttackState::BossMonsterComboAttackState()
	:mAttackElapsedTime(0.f)
	, mEmitAttackTime(0.f)
	, mXAxisOffset(1.f)
{}

fq::client::BossMonsterComboAttackState::~BossMonsterComboAttackState()
{}

void fq::client::BossMonsterComboAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	auto bossMonster = animator.GetComponent<BossMonster>();
	
	bossMonster->ReboundComboAttack();

	mPrevPlayBackSpeed = state.GetPlayBackSpeed();

	if (bossMonster->IsAngry())
	{
		state.SetPlayBackSpeed(mPrevPlayBackSpeed * bossMonster->GetAngryAttackSpeedRatio());
	}
}

void fq::client::BossMonsterComboAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mAttackElapsedTime != mEmitAttackTime)
	{
		mAttackElapsedTime += dt;

		if (mAttackElapsedTime >= mEmitAttackTime)
		{
			animator.GetComponent<BossMonster>()->EmitComboAttack(mXAxisOffset);
			animator.GetComponent<BossMonster>()->EmitComboEffect(mXAxisOffset);
			mAttackElapsedTime = mEmitAttackTime;
		}
	}
}

void fq::client::BossMonsterComboAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

	bool isOnCombo3 = animator.GetController().GetParameter("OnCombo3").cast<bool>();

	if (state.GetAnimationKey() == "ComboAttack3" && isOnCombo3)
	{
		animator.GetComponent<BossMonster>()->EndPattern();
	}
	else if (state.GetAnimationKey() == "ComboAttack2" && !isOnCombo3)
	{
		animator.GetComponent<BossMonster>()->EndPattern();
	}
	
	state.SetPlayBackSpeed(mPrevPlayBackSpeed);
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterComboAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterComboAttackState>(*this);
}
