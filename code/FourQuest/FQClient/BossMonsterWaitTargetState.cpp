#include "BossMonsterWaitTargetState.h"

#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

fq::client::BossMonsterWaitTargetState::BossMonsterWaitTargetState()
{

}

fq::client::BossMonsterWaitTargetState::~BossMonsterWaitTargetState()
{

}

void fq::client::BossMonsterWaitTargetState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	animator.GetComponent<BossMonster>()->SetRandomTarget();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterWaitTargetState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterWaitTargetState>(*this);
}

void fq::client::BossMonsterWaitTargetState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}
