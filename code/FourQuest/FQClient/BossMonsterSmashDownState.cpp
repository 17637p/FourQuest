#include "BossMonsterSmashDownState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterSmashDownState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterSmashDownState>(*this);
}

fq::client::BossMonsterSmashDownState::~BossMonsterSmashDownState()
{

}

fq::client::BossMonsterSmashDownState::BossMonsterSmashDownState()
	:mEffectEmitTime(1.f)
	, mEffectElapsedTime(0.f)
	, mAttackElapsedTime(0.f)
	, mAttackEmitTime(1.f)
	, mEffect(nullptr)
	, mHomingTime(0.5f)
	, mHomingElapsedTime(0.f)
{

}

void fq::client::BossMonsterSmashDownState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss =  animator.GetComponent<BossMonster>();

	mHomingElapsedTime += dt;
	if (mHomingElapsedTime <= mHomingTime)
	{
		boss->HomingTarget();
	}

	// effect 
	if (mEffectElapsedTime != mEffectEmitTime)
	{
		mEffectElapsedTime += dt * state.GetPlayBackSpeed();

		if (mEffectElapsedTime >= mEffectEmitTime)
		{
			mEffectElapsedTime = mEffectEmitTime;

			mEffect = boss->EmitSmashDownEffect();
		}
	}

	// attack
	if (mAttackEmitTime != mAttackElapsedTime)
	{
		mAttackElapsedTime += dt * state.GetPlayBackSpeed();

		if (mAttackElapsedTime >= mAttackEmitTime)
		{
			mAttackElapsedTime = mAttackEmitTime;
			boss->EmitSmashDown();
		}
	}
}

void fq::client::BossMonsterSmashDownState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	mEffectElapsedTime = 0.f;
	mHomingElapsedTime = 0.f;

	animator.GetComponent<game_module::NavigationAgent>()->Stop();
}

void fq::client::BossMonsterSmashDownState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mEffect)
	{
		animator.GetScene()->DestroyGameObject(mEffect.get());
		mEffect = nullptr;
	}

	animator.GetComponent<BossMonster>()->EndPattern();
}