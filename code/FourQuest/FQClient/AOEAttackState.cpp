#include "AOEAttackState.h"

#include "../FQGameModule/GameModule.h"

fq::client::AOEAttackState::AOEAttackState()
{

}

fq::client::AOEAttackState::~AOEAttackState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::AOEAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<AOEAttackState>(*this);
}

void fq::client::AOEAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{


}

void fq::client::AOEAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
