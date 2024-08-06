#include "MeleeMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::MeleeMonsterHitState::MeleeMonsterHitState()
{

}

fq::client::MeleeMonsterHitState::~MeleeMonsterHitState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterHitState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterHitState>(*this);
}

void fq::client::MeleeMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}
