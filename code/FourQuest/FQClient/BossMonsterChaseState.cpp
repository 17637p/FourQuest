#include "BossMonsterChaseState.h"

fq::client::BossMonsterChaseState::BossMonsterChaseState()
{

}

fq::client::BossMonsterChaseState::~BossMonsterChaseState()
{

}

void fq::client::BossMonsterChaseState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

void fq::client::BossMonsterChaseState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterChaseState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterChaseState>(*this);
}
