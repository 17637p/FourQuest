#include "BerserkerRushState.h"
#include "BerserkerArmour.h"
#include "Attack.h"
#include "Player.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BerserkerRushState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BerserkerRushState>(*this);
}

void fq::client::BerserkerRushState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetPadInputRotation(fq::game_module::EPadStickType::Right);
	controller->SetDashInput(true);

	mElapsedTime = 0.f;
	mAccumulatedTime = 0.f;
	mAttackObjectOrNull = nullptr;

	auto playerOrNull = animator.GetComponent<Player>();
	if (playerOrNull != nullptr)
	{
		playerOrNull->SetIsActiveOnHit(false);
	}
}

void fq::client::BerserkerRushState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mAccumulatedTime += dt;

	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();
	auto foward = transform->GetWorldMatrix().Forward();

	float velocity = std::min<float>(mMaxVelocity, mVelocity + mAcceleration * mAccumulatedTime);

	foward.Normalize();
	foward.x *= velocity;
	foward.z *= velocity;
	rigid->SetLinearVelocity(foward);

	if (mAttackObjectOrNull != nullptr)
	{
		auto attackComponent = mAttackObjectOrNull->GetComponent<client::Attack>();
		attackComponent->SetAttackPosition(transform->GetWorldPosition());
	}

	if (mElapsedTime == mAttackTiming)
	{
		return;
	}

	mElapsedTime = std::min<float>(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mElapsedTime == mAttackTiming)
	{
		auto berserkerArmour = animator.GetComponent<BerserkerArmour>();
		mAttackObjectOrNull = berserkerArmour->EmitAttackIntend(mAttackType, mColliderOffset, mColliderScale, mKnocBackPower, mDestroyTime);
	}
}

void fq::client::BerserkerRushState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	BerserkerAttackState::OnStateExit(animator, state);

	if (mAttackObjectOrNull != nullptr)
	{
		mAttackObjectOrNull->OnDestroy();
	}

	auto playerOrNull = animator.GetComponent<Player>();
	if (playerOrNull != nullptr)
	{
		playerOrNull->SetIsActiveOnHit(true);
	}
}
