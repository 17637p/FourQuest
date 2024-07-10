#define NOMINMAX
#include "MeleeMonsterFindTargetState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

fq::client::MeleeMonsterFindTargetState::MeleeMonsterFindTargetState()
	:mAnnounceTime(0.2f)
	, mElapsedTime(0.f)
{

}

fq::client::MeleeMonsterFindTargetState::~MeleeMonsterFindTargetState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterFindTargetState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterFindTargetState>(*this);
}

void fq::client::MeleeMonsterFindTargetState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;
}

void fq::client::MeleeMonsterFindTargetState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.SetParameterBoolean("FindTarget", false);
}

void fq::client::MeleeMonsterFindTargetState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAnnounceTime)
		return;

	mElapsedTime = std::min(mElapsedTime + dt, mAnnounceTime);

	if (mAnnounceTime == mElapsedTime)
	{
		animator.GetComponent<MeleeMonster>()->AnnounceFindedTarget();
	}
}
