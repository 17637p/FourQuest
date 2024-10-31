#include "BossMonsterPrepareAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

fq::client::BossMonsterPrepareAttackState::BossMonsterPrepareAttackState()
	: mHomingElapsedTime(0.f)
	, mHomingTime(0.f)
	, mAttackType()
	, mEffectXOffset(0.f)
	, mbUseStepBack(false)
	, mbUseRebind(false)
	, mbUseRotationSpeed(true)
{}

fq::client::BossMonsterPrepareAttackState::~BossMonsterPrepareAttackState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterPrepareAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterPrepareAttackState>(*this);
}

void fq::client::BossMonsterPrepareAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<game_module::NavigationAgent>()->Stop();
	mHomingElapsedTime = 0.f;

	auto bossMonster = animator.GetComponent<BossMonster>();

	if (mbUseStepBack)
	{
		bossMonster->StepBack();
	}

	if (mbUseRebind)
	{
		bossMonster->ReboundComboAttack();
	}

	mPrevPlayBackSpeed = state.GetPlayBackSpeed();

	if (bossMonster->IsAngry())
	{
		state.SetPlayBackSpeed(mPrevPlayBackSpeed * bossMonster->GetAngryPreAttackSpeedRatio());
	}
}

void fq::client::BossMonsterPrepareAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto bossMonster = animator.GetComponent<BossMonster>();

	mHomingElapsedTime += dt * state.GetPlayBackSpeed();

	if (mHomingElapsedTime < mHomingTime)
	{
		if (bossMonster != nullptr)
		{
			bossMonster->HomingTarget(mbUseRotationSpeed);
		}
	}
	else
	{
		if (bossMonster != nullptr)
		{
			if (mEffectObject != nullptr)
			{
				switch (mAttackType)
				{
				case fq::client::EBossMonsterAttackType::Combo:
					bossMonster->SetComboDecalEffect(mEffectObject, mEffectXOffset);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (mAttackType)
				{
				case fq::client::EBossMonsterAttackType::Rush:
					mEffectObject = bossMonster->EmitRushDecalEffect();
					break;
				case fq::client::EBossMonsterAttackType::SmashDown:
					mEffectObject = bossMonster->EmitSmashDecalEffect();
					break;
				case fq::client::EBossMonsterAttackType::Combo:
					mEffectObject = bossMonster->EmitComboDecalEffect(mEffectXOffset);
					break;
				default:
					break;
				}
			}
		}
	}
}

void fq::client::BossMonsterPrepareAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mEffectObject != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mEffectObject.get());
		mEffectObject = nullptr;
	}

	state.SetPlayBackSpeed(mPrevPlayBackSpeed);
}

