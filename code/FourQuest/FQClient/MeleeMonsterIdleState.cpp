#include "MeleeMonsterIdleState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterIdleState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterIdleState>(*this);
}

fq::client::MeleeMonsterIdleState::MeleeMonsterIdleState()
	:mElapsedTime(0.f)
{

}

fq::client::MeleeMonsterIdleState::~MeleeMonsterIdleState()
{

}

void fq::client::MeleeMonsterIdleState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsedTime += dt;

	auto melee = animator.GetComponent<MeleeMonster>();


	if (mElapsedTime >= 1.f)
	{
		melee->ChangeToRandomIdleOrPatrol();
	} 
}

void fq::client::MeleeMonsterIdleState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;

}
