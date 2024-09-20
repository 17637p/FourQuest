#include "BossMonsterContinousState.h"

#include "../FQGameModule/GameModule.h"
#include "BossMonster.h"

fq::client::BossMonsterContinousState::BossMonsterContinousState()
	:mSecondAttackTime(1.5f)
	, mFirstAttackTime(0.6f)
	, mAttackDuration(10.f)
	, mAttackElapsedTime(0.f)
	, mAttackType(EAttackType::First)
{

}

fq::client::BossMonsterContinousState::~BossMonsterContinousState()
{

}

void fq::client::BossMonsterContinousState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	mAttackType = EAttackType::First;
}

void fq::client::BossMonsterContinousState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss = animator.GetComponent<BossMonster>();
	boss->HomingTarget();

	auto timePos = animator.GetController().GetTimePos();

	if (timePos >= mFirstAttackTime && mAttackType == EAttackType::First)
	{
		// 첫번째 공격
		mAttackType = EAttackType::Second;
		boss->ReboundComboAttack();
		boss->EmitComboAttack(0.f);
	}
	else if (timePos >= mSecondAttackTime && mAttackType == EAttackType::Second)
	{
		// 두번째 공격
		mAttackType = EAttackType::InitReady;
		boss->ReboundComboAttack();
		boss->EmitComboAttack(0.f);
	}
	else if (timePos < mFirstAttackTime && mAttackType == EAttackType::InitReady)
	{
		// Loof 애니메이션 초기화 
		mAttackType = EAttackType::First;
	}

	// 공격 지속시간 
	mAttackElapsedTime += dt;
	if (mAttackElapsedTime >= mAttackDuration)
	{
		boss->SetTarget(nullptr);
	}
}

void fq::client::BossMonsterContinousState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<BossMonster>()->EndPattern();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterContinousState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterContinousState>(*this);

}
