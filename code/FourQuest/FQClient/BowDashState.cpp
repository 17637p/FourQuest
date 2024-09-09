#include "BowDashState.h"

namespace fq::client
{
	BowDashState::BowDashState()
	{
	}
	BowDashState::~BowDashState()
	{
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BowDashState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowDashState>(*this);
	}
	void BowDashState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
	void BowDashState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
	}
	void BowDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
}
