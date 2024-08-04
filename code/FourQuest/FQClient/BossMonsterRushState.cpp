#include "BossMonsterRushState.h"

fq::client::BossMonsterRushState::BossMonsterRushState()
{

}

fq::client::BossMonsterRushState::~BossMonsterRushState()
{

}

void fq::client::BossMonsterRushState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

void fq::client::BossMonsterRushState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterRushState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterRushState>(*this);
}
