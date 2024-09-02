#include "BowSoulAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"
#include "PlayerSoulVariable.h"

fq::client::BowSoulAttackState::BowSoulAttackState()
	: mAttackElapsedTime(0.f)
{

}

fq::client::BowSoulAttackState::~BowSoulAttackState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BowSoulAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BowSoulAttackState>(*this);
}

void fq::client::BowSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mAttackElapsedTime = 0.f;
	mAttackDuration = 0.f;

	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
}

void fq::client::BowSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipArmourWeapone();
	animator.SetParameterBoolean("EndSoulAttack", false);
}

void fq::client::BowSoulAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	assert(PlayerSoulVariable::SoulBowAttackTick > 0.f);

	mAttackElapsedTime += dt;
	mAttackDuration += dt;

	while (mAttackElapsedTime >= PlayerSoulVariable::SoulBowAttackTick)
	{
		mAttackElapsedTime -= PlayerSoulVariable::SoulBowAttackTick;
		auto player = animator.GetComponent<Player>();
		player->EmitBowSoulAttack();
	}

	if (mAttackDuration >= PlayerSoulVariable::SoulBowAttackDuration)
	{
		animator.SetParameterBoolean("EndSoulAttack", true);
	}
}
