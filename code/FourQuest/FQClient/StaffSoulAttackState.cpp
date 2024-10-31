#include "StaffSoulAttackState.h"

#include "Player.h"
#include "../FQGameModule/GameModule.h"
#include "PlayerSoulVariable.h"

fq::client::StaffSoulAttackState::StaffSoulAttackState()
{

}

fq::client::StaffSoulAttackState::~StaffSoulAttackState()
{

}

void fq::client::StaffSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();

	if (player != nullptr)
	{
		player->EquipSoulWeapone();
		player->AddSoulGauge(-PlayerSoulVariable::SoulSwordAttackCost);
		player->SetIsActiveOnHit(false);
	}
}

void fq::client::StaffSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();

	if (player != nullptr)
	{
		player->EmitStaffSoulAttack();
		player->EquipArmourWeapone();
		player->SetIsActiveOnHit(true);
	}
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::StaffSoulAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<StaffSoulAttackState>(*this);
}

