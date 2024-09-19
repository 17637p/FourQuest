#include "PlantMonsterAttckState.h"

#include "../FQGameModule/GameModule.h"
#include "PlantMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterAttckState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterAttckState>(*this);

}

fq::client::PlantMonsterAttckState::PlantMonsterAttckState()
	:mAttackTiming(0.f)
	, mElapsedTime(0.f)
	, mLookAtTime(0.5f)
	, mLookAtElapsedTime(0.f)
{}

fq::client::PlantMonsterAttckState::~PlantMonsterAttckState()
{}

void fq::client::PlantMonsterAttckState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mLookAtElapsedTime = 0.f;
	mElapsedTime = 0.f;

	// ���� ��� �ߵ�
	if (mAttackTiming == mElapsedTime)
	{
		animator.GetComponent<PlantMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;
	}
}

void fq::client::PlantMonsterAttckState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
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

	// ���� �ߵ� 
	if (mElapsedTime >= mAttackTiming)
	{
		animator.GetComponent<PlantMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;
	}
}

