#include "MeleeMonsterPatrolState.h"

#include "../FQGameModule/NavigationAgent.h"
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

void fq::client::MeleeMonsterPatrolState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto monster = animator.GetComponent<MeleeMonster>();
	monster->Patrol();
}

void fq::client::MeleeMonsterPatrolState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	// 목표 지점에 도착한지 확인합니다 
	auto agent = animator.GetComponent<game_module::NavigationAgent>();

	if (agent->HasReachedDestination())
	{
		animator.SetParameterBoolean("OnPatrol", false);
	}

	auto monster = animator.GetComponent<MeleeMonster>();
	monster->DetectTarget();
}

