#include "PlayerMovementState.h"

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/GameModule.h"

fq::client::PlayerMovementState::PlayerMovementState()
	:mbCanMovePlayer(false)
	,mbOnRotation(true)
{}

fq::client::PlayerMovementState::~PlayerMovementState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerMovementState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerMovementState>(*this);
}

void fq::client::PlayerMovementState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto controller = animator.GetComponent<game_module::CharacterController>();

	controller->SetCanMoveCharater(mbCanMovePlayer);
	controller->SetOnRotation(mbOnRotation);
}