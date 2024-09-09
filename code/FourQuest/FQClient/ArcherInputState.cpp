#include "ArcherInputState.h"

namespace fq::client
{
	ArcherInputState::ArcherInputState()
	{
	}
	ArcherInputState::~ArcherInputState()
	{
	}

	void ArcherInputState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}
	void ArcherInputState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
	}
	void ArcherInputState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}



	std::shared_ptr<fq::game_module::IStateBehaviour> ArcherInputState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<ArcherInputState>(*this);
	}
	entt::meta_handle ArcherInputState::GetHandle()
	{
		return *this;
	}
}
