#include "PlayerLowerMovementState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

fq::client::PlayerLowerMovementState::PlayerLowerMovementState()
	:mbOffExitToLowerMovement(false)
	,mbOnEnterToLowerMovement(false)
{}

fq::client::PlayerLowerMovementState::~PlayerLowerMovementState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerLowerMovementState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerLowerMovementState>(*this);
}

void fq::client::PlayerLowerMovementState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	if (mbOnEnterToLowerMovement)
	{
		animator.SetUseLowerController(true);
	}
}

void fq::client::PlayerLowerMovementState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	animator.GetComponent<Player>()->SetLowerBodyAnimation();
}

void fq::client::PlayerLowerMovementState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	if (mbOffExitToLowerMovement)
	{
		animator.SetUseLowerController(false);
	}
}
