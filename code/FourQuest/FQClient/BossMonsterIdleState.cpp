#include "BossMonsterIdleState.h"

#include "BossMonster.h"

fq::client::BossMonsterIdleState::~BossMonsterIdleState()
{

}

fq::client::BossMonsterIdleState::BossMonsterIdleState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterIdleState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterIdleState>(*this);
}

void fq::client::BossMonsterIdleState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

void fq::client::BossMonsterIdleState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss = animator.GetComponent<BossMonster>();
	boss->DetectTarget();
}

