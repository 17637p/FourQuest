#include "MeleeMonsterPatrolState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterPatrolState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterPatrolState>(*this);

}

fq::client::MeleeMonsterPatrolState::~MeleeMonsterPatrolState()
{

}

fq::client::MeleeMonsterPatrolState::MeleeMonsterPatrolState()
{

}

void fq::client::MeleeMonsterPatrolState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<MeleeMonster>();
	monster->Patrol();
}
