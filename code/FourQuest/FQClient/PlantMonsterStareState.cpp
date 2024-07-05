#include "PlantMonsterStareState.h"

#include "../FQGameModule/GameModule.h"
#include "PlantMonster.h"

fq::client::PlantMonsterStareState::PlantMonsterStareState()
{

}

fq::client::PlantMonsterStareState::~PlantMonsterStareState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterStareState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterStareState>(*this);
}

void fq::client::PlantMonsterStareState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<PlantMonster>();

	monster->LookAtTarget();
	monster->CheckTargetInAttackRange();
	monster->CheckAttackAble();
}
