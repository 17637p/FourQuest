#include "PlayerAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

fq::client::PlayerAttackState::~PlayerAttackState()
{}

fq::client::PlayerAttackState::PlayerAttackState()
	: mAttackRebound(0.f)
	, mAttackTiming(0.f)
	, mElapsedTime(0.f)
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerAttackState>(*this);
}

void fq::client::PlayerAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 컨트롤러 입력방향을 바라봅니다
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();

	auto foward = animator.GetComponent<game_module::Transform>()->GetLocalMatrix().Forward();
	foward.Normalize();
	animator.GetComponent<game_module::RigidBody>()->AddLinearVelocity(foward * mAttackRebound);

	mElapsedTime = 0.f;

	// 공격 즉시 발동
	if (mAttackTiming == mElapsedTime)
	{
		animator.GetComponent<Player>()->Attack();
	}
}

void fq::client::PlayerAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
		return;

	mElapsedTime += dt;

	// 공격 발동 
	if (mElapsedTime >= mAttackTiming)
	{
		animator.GetComponent<Player>()->Attack();
		mElapsedTime = mAttackTiming;
	}
}
