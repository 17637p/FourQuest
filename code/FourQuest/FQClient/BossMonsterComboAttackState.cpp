#include "BossMonsterComboAttackState.h"

#include "BossMonster.h"

fq::client::BossMonsterComboAttackState::BossMonsterComboAttackState()
{}

fq::client::BossMonsterComboAttackState::~BossMonsterComboAttackState()
{}

void fq::client::BossMonsterComboAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

void fq::client::BossMonsterComboAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{}

void fq::client::BossMonsterComboAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterComboAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterComboAttackState>(*this);
}
