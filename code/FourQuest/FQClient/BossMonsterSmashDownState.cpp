#include "BossMonsterSmashDownState.h"

#include "../FQGameModule/GameModule.h"
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
{

}

void fq::client::BossMonsterSmashDownState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	// effect 
	if (mEffectElapsedTime != mEffectEmitTime)
	{
		mEffectElapsedTime += dt * state.GetPlayBackSpeed();

		if (mEffectElapsedTime >= mEffectEmitTime)
		{
			mEffectElapsedTime = mEffectEmitTime;

			mEffect = animator.GetComponent<BossMonster>()->EmitSmashDownEffect();
		}
	}

	// attack
	if (mAttackEmitTime != mAttackElapsedTime)
	{
		mAttackElapsedTime += dt * state.GetPlayBackSpeed();

		if (mAttackElapsedTime >= mAttackEmitTime)
		{
			mAttackElapsedTime = mAttackEmitTime;
			animator.GetComponent<BossMonster>()->SmashDown();
		}
	}
}

void fq::client::BossMonsterSmashDownState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	mEffectElapsedTime = 0.f;
}

void fq::client::BossMonsterSmashDownState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mEffect)
	{
		animator.GetScene()->DestroyGameObject(mEffect.get());
		mEffect = nullptr;
	}


}