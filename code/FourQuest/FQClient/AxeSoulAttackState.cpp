#include "AxeSoulAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"
#include "PlayerSoulVariable.h"

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
	mAttackElapsedTime += dt;
	mAttackDuration += dt;

	while (mAttackElapsedTime >= PlayerSoulVariable::SoulAxeAttackTick)
	{
		mAttackElapsedTime -= PlayerSoulVariable::SoulAxeAttackTick;
		auto player = animator.GetComponent<Player>();
		player->EmitAxeSoulAttack();
	}

	if (mAttackDuration >= PlayerSoulVariable::SoulAxeAttackDuration)
	{
		animator.SetParameterBoolean("EndSoulAttack", true);
	}

}

void fq::client::AxeSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
	mAttackElapsedTime = 0.f;
	mAttackDuration = 0.f;
}

void fq::client::AxeSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipArmourWeapone();
	animator.SetParameterBoolean("EndSoulAttack", false);
}
