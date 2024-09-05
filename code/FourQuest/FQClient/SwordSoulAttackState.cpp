#define  NOMINMAX
#include "SwordSoulAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/Transform.h"
#include "Player.h"
#include "PlayerSoulVariable.h"

fq::client::SwordSoulAttackState::SwordSoulAttackState()
	:mAttackTiming(1.f)
	, mElapsedTime(0.f)
{}

fq::client::SwordSoulAttackState::~SwordSoulAttackState()
{

}

void fq::client::SwordSoulAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 컨트롤러 입력방향을 바라봅니다
	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetPadInputRotation();

	mElapsedTime = 0.f;

	// 검을 장착합니다
	auto player = animator.GetComponent<Player>();
	player->EquipSoulWeapone();
	player->AddSoulGauge(-PlayerSoulVariable::SoulSwordAttackCost);
}

void fq::client::SwordSoulAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
	{
		return;
	}

	mElapsedTime = std::min(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mElapsedTime == mAttackTiming)
	{
		auto player = animator.GetComponent<Player>();
		player->EmitSwordSoulAttack();
	}

}

void fq::client::SwordSoulAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto player = animator.GetComponent<Player>();
	player->EquipArmourWeapone();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::SwordSoulAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<SwordSoulAttackState>(*this);
}
