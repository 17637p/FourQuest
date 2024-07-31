#include "BossMonsterFindTargetState.h"

#include "../FQGameModule/GameModule.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterFindTargetState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterFindTargetState>(*this);
}

fq::client::BossMonsterFindTargetState::BossMonsterFindTargetState()
{

}

fq::client::BossMonsterFindTargetState::~BossMonsterFindTargetState()
{

}

void fq::client::BossMonsterFindTargetState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MeleeMonsterFind", false , 0 });
}

void fq::client::BossMonsterFindTargetState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.SetParameterBoolean("FindTarget", false);
}
