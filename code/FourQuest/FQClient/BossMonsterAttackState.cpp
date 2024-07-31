#include "BossMonsterAttackState.h"

void fq::client::BossMonsterAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

void fq::client::BossMonsterAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterAttackState>(*this);
}

fq::client::BossMonsterAttackState::~BossMonsterAttackState()
{

}

fq::client::BossMonsterAttackState::BossMonsterAttackState()
{

}
