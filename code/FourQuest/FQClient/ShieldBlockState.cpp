#include "ShieldBlockState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "KnightArmour.h"
#include "Player.h"
#include "PlayerVariable.h"

fq::client::ShieldBlockState::ShieldBlockState()
{}

fq::client::ShieldBlockState::~ShieldBlockState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::ShieldBlockState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<ShieldBlockState>(*this);
}

void fq::client::ShieldBlockState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetPadInputRotationBySpeed(game_module::EPadStickType::Right, PlayerVariable::PadRotationSpeed, dt);
	animator.GetComponent<KnightArmour>()->UpdateBlockState(dt);
	animator.GetComponent<Player>()->SetLowerBodyAnimation();
}

void fq::client::ShieldBlockState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	animator.GetComponent<KnightArmour>()->EnterShieldState();
}

void fq::client::ShieldBlockState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto nextState = animator.GetController().GetNextStateName();
	if (nextState == "Hit" || nextState == "Idle")
	{
		auto knight = animator.GetComponent<KnightArmour>();
		knight->ExitShieldState();
	}

}
