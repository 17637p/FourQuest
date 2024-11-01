#include "BowPullState.h"

#include "ArcherArmour.h"

namespace fq::client
{
	BowPullState::BowPullState()
	{
	}

	BowPullState::~BowPullState()
	{
	}

	void BowPullState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		auto archer = animator.GetComponent<ArcherArmour>();
		archer->MakeLineOfSight();
	}

	void BowPullState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
	}

	void BowPullState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BowPullState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowPullState>(*this);
	}

	entt::meta_handle BowPullState::GetHandle()
	{
		return *this;
	}

}
