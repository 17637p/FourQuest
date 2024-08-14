#include "BowSoulAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

fq::client::BowSoulAttackState::BowSoulAttackState()
	:mAttackTick(0.1f)
	, mAttackElapsedTime(0.f)
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

	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
	mAttackTick = player->GetBowAttackTick();
}

void fq::client::BowSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipArmourWeapone();
}

void fq::client::BowSoulAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mAttackElapsedTime += dt;

	while (mAttackElapsedTime >= mAttackTick)
	{
		mAttackElapsedTime -= mAttackTick;
		auto player = animator.GetComponent<Player>();
		player->EmitBowSoulAttack();
	}
}
