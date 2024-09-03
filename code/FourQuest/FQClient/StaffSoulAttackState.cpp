#include "StaffSoulAttackState.h"

#include "Player.h"
#include "../FQGameModule/GameModule.h"

fq::client::StaffSoulAttackState::StaffSoulAttackState()
{

}

fq::client::StaffSoulAttackState::~StaffSoulAttackState()
{

}

void fq::client::StaffSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EmitStaffSoulAttack();
	player->EquipArmourWeapone();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::StaffSoulAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<StaffSoulAttackState>(*this);
}

void fq::client::StaffSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
}
