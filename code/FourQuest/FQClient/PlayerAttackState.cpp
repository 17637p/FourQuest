#include "PlayerAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

fq::client::PlayerAttackState::~PlayerAttackState()
{

}

fq::client::PlayerAttackState::PlayerAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerAttackState>(*this);
}

void fq::client::PlayerAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<Player>()->Attack();
}
