#define NOMINMAX
#include "MagicBallAttackState.h"

#include <algorithm>

#include "../FQGameModule/CharacterController.h"
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
	// 컨트롤러 입력방향을 바라봅니다
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();

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
}

void fq::client::MagicBallAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
