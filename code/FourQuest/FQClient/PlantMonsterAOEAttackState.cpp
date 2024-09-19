#include "PlantMonsterAOEAttackState.h"

#include "PlantMonster.h"

namespace fq::client
{
	PlantMonsterAOEAttackState::PlantMonsterAOEAttackState()
		: mLookAtTime()
		, mLookAtElapsedTime()
		, mAttackTiming()
		, mElapsedTime()
	{
	}
	PlantMonsterAOEAttackState::~PlantMonsterAOEAttackState()
	{
	}

	void PlantMonsterAOEAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		mLookAtElapsedTime = 0.f;
		mElapsedTime = 0.f;

		// 공격 즉시 발동
		if (mAttackTiming == mElapsedTime)
		{
			animator.GetComponent<PlantMonster>()->EmitAOEAttack();
			mElapsedTime = mAttackTiming;
		}
	}

	void PlantMonsterAOEAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
	{
		if (mLookAtElapsedTime != mLookAtTime)
		{
			mLookAtElapsedTime += dt;
			animator.GetComponent<PlantMonster>()->LookAtTarget();

			if (mLookAtElapsedTime >= mLookAtTime)
			{
				mLookAtElapsedTime = mLookAtTime;
			}
		}

		if (mElapsedTime == mAttackTiming)
			return;

		mElapsedTime += dt;

		// 공격 발동 
		if (mElapsedTime >= mAttackTiming)
		{
			animator.GetComponent<PlantMonster>()->EmitAOEAttack();
			mElapsedTime = mAttackTiming;
		}
	}



	std::shared_ptr<fq::game_module::IStateBehaviour> PlantMonsterAOEAttackState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterAOEAttackState>(*this);

	}
}
