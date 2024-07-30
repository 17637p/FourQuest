#include "PlayerInputState.h"

#include "../FQGameModule/GameModule.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerInputState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerInputState>(*this);
}

fq::client::PlayerInputState::PlayerInputState()
{}

fq::client::PlayerInputState::~PlayerInputState()
{}

void fq::client::PlayerInputState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto controller = animator.GetComponent<game_module::CharacterController>();
	auto controllerID = controller->GetControllerID();
	auto input = animator.GetGameObject()->GetScene()->GetInputManager();

	if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::Tap))
	{
		animator.SetParameterTrigger("PushX");
	}

	if (input->IsPadKeyState(controllerID, EPadKey::Y, EKeyState::Tap))
	{
		animator.SetParameterTrigger("PushY");
	}
}

void fq::client::PlayerInputState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	animator.SetParameterOffTrigger("PushX");
	animator.SetParameterOffTrigger("PushY");
}

void fq::client::PlayerInputState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
}
