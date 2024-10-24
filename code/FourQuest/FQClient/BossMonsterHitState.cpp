#include "BossMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterHitState::BossMonsterHitState()
{}

fq::client::BossMonsterHitState::~BossMonsterHitState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterHitState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterHitState>(*this);
}

void fq::client::BossMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });

	animator.SetParameterBoolean("IsAnyState", false);
}

void fq::client::BossMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.SetParameterBoolean("IsAnyState", true);
	animator.SetParameterBoolean("OnHitRight", false);
	animator.SetParameterBoolean("OnHitLeft", false);
	animator.SetParameterBoolean("OnHitFront", false);
	animator.SetParameterBoolean("OnHitBack", false);
}

