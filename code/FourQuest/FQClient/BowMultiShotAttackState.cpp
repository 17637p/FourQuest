#include "BowMultiShotAttackState.h"

namespace fq::client
{
	BowMultiShotAttackState::BowMultiShotAttackState()
	{
	}
	BowMultiShotAttackState::~BowMultiShotAttackState()
	{
	}
	std::shared_ptr<fq::game_module::IStateBehaviour> BowMultiShotAttackState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowMultiShotAttackState>(*this);
	}
	void BowMultiShotAttackState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
	void BowMultiShotAttackState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
	}
	void BowMultiShotAttackState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
}
