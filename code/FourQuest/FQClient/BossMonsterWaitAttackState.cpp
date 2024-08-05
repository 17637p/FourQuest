#include "BossMonsterWaitAttackState.h"

#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterWaitAttackState::BossMonsterWaitAttackState()
{

}

fq::client::BossMonsterWaitAttackState::~BossMonsterWaitAttackState()
{

}

void fq::client::BossMonsterWaitAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}

void fq::client::BossMonsterWaitAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterWaitAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterWaitAttackState>(*this);
}
