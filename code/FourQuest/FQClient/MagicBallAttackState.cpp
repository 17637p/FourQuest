#define NOMINMAX
#include "MagicBallAttackState.h"

#include <algorithm>

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::MagicBallAttackState::MagicBallAttackState()
	:mAttackElapsedTime(0.f)
	, mAttackTiming(0.f)
	, mCheckInputElapsedTime(0.f)
	, mCheckInputTime(0.5f)
{}

fq::client::MagicBallAttackState::~MagicBallAttackState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MagicBallAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MagicBallAttackState>(*this);
}

void fq::client::MagicBallAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 컨트롤러 입력방향을 바라봅니다
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();
	animator.GetComponent<MagicArmour>()->AimToNearMonster();

	mAttackElapsedTime = 0.f;
	mCheckInputElapsedTime = 0.f;
}

void fq::client::MagicBallAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	checkAttackEmit(animator, state, dt);

	mCheckInputElapsedTime += dt;

	if (mCheckInputElapsedTime >= mCheckInputTime)
	{
		// 마법 기본 공격 X키 홀드 로직 추가
		auto input = animator.GetGameObject()->GetScene()->GetInputManager();
		auto controller = animator.GetComponent<game_module::CharacterController>();
		auto controllerID = controller->GetControllerID();

		if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::Hold))
		{
			animator.SetParameterTrigger("PushX");
		}
	}
}

void fq::client::MagicBallAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

void fq::client::MagicBallAttackState::checkAttackEmit(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mAttackElapsedTime == mAttackTiming)
	{
		return;
	}

	mAttackElapsedTime = std::min(mAttackElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mAttackElapsedTime == mAttackTiming)
	{
		auto magicArmour = animator.GetComponent<MagicArmour>();
		magicArmour->EmitMagicBall();
	}

}
