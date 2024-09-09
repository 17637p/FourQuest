#define NOMINMAX
#include "MagicBallAttackState.h"

#include <algorithm>

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::MagicBallAttackState::MagicBallAttackState()
	:mElapsedTime(0.f)
	, mAttackTiming(0.f)
{

}

fq::client::MagicBallAttackState::~MagicBallAttackState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MagicBallAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MagicBallAttackState>(*this);

}

void fq::client::MagicBallAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// ��Ʈ�ѷ� �Է¹����� �ٶ󺾴ϴ�
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();
	
	animator.GetComponent<MagicArmour>()->AimToNearMonster();

	mElapsedTime = 0.f;
}

void fq::client::MagicBallAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
	{
		return;
	}

	mElapsedTime = std::min(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mElapsedTime == mAttackTiming)
	{
		auto magicArmour = animator.GetComponent<MagicArmour>();
		magicArmour->EmitMagicBall();
	}

	// ���� �⺻ ���� XŰ Ȧ�� ���� �߰�
	auto input = animator.GetGameObject()->GetScene()->GetInputManager();
	auto controller = animator.GetComponent<game_module::CharacterController>();
	auto controllerID = controller->GetControllerID();

	if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::Hold))
	{
		animator.SetParameterTrigger("PushX");
	}
}

void fq::client::MagicBallAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
