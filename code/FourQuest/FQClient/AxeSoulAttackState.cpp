#include "AxeSoulAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

fq::client::AxeSoulAttackState::AxeSoulAttackState()
{

}

fq::client::AxeSoulAttackState::~AxeSoulAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::AxeSoulAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<AxeSoulAttackState>(*this);
}

void fq::client::AxeSoulAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::AxeSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
}

void fq::client::AxeSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipArmourWeapone();
}
