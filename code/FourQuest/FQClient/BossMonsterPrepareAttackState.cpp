#include "BossMonsterPrepareAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

fq::client::BossMonsterPrepareAttackState::BossMonsterPrepareAttackState()
	:mHomingElapsedTime(0.f)
	, mHomingTime(0.f)
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
}

void fq::client::BossMonsterPrepareAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mHomingElapsedTime += dt * state.GetPlayBackSpeed();

	if (mHomingElapsedTime < mHomingTime)	
	{
		animator.GetComponent<BossMonster>()->HomingTarget();
	}
}

void fq::client::BossMonsterPrepareAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

