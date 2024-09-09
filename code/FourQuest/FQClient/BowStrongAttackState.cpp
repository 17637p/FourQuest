#include "BowStrongAttackState.h"

namespace fq::client
{
	BowStrongAttackState::BowStrongAttackState()
	{
	}
	BowStrongAttackState::~BowStrongAttackState()
	{
	}
	std::shared_ptr<fq::game_module::IStateBehaviour> BowStrongAttackState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowStrongAttackState>(*this);
	}
	void BowStrongAttackState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
	void BowStrongAttackState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
	}
	void BowStrongAttackState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
}
