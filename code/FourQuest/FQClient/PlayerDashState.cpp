#include "PlayerDashState.h"

#include "../FQGameModule/GameModule.h"

fq::client::PlayerDashState::~PlayerDashState()
{

}

fq::client::PlayerDashState::PlayerDashState()
	:mDashPower(0.f)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerDashState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerDashState>(*this);
}

void fq::client::PlayerDashState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();

	auto foward = transform->GetWorldMatrix().Forward();

	foward.Normalize();

	foward.x *= mDashPower;
	foward.z *= mDashPower;

	rigid->SetLinearVelocity(foward);
}

void fq::client::PlayerDashState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::PlayerDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}
