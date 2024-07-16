#include "RazerAttackState.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::RazerAttackState::RazerAttackState()
{

}

fq::client::RazerAttackState::~RazerAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::RazerAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<RazerAttackState>(*this);
}

void fq::client::RazerAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

void fq::client::RazerAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::RazerAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
