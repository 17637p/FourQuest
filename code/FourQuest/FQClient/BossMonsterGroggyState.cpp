#include "BossMonsterGroggyState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterGroggyState::BossMonsterGroggyState()
{}

fq::client::BossMonsterGroggyState::~BossMonsterGroggyState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterGroggyState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterGroggyState>(*this);
}

void fq::client::BossMonsterGroggyState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });

	animator.SetParameterBoolean("OnHitRight", false);
	animator.SetParameterBoolean("OnHitLeft", false);
	animator.SetParameterBoolean("OnHitFront", false);
	animator.SetParameterBoolean("OnHitBack", false);
}

