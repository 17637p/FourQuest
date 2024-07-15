#include "MeleeMonsterChaseState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

fq::client::MeleeMonsterChaseState::~MeleeMonsterChaseState()
{

}

fq::client::MeleeMonsterChaseState::MeleeMonsterChaseState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterChaseState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterChaseState>(*this);
}

void fq::client::MeleeMonsterChaseState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<MeleeMonster>();
	monster->ChaseTarget();
	monster->CheckTargetInAttackRange();
}
