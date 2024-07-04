#include "PlantMonsterIdleState.h"
#include "../FQGameModule/GameModule.h"

#include "PlantMonster.h"

fq::client::PlantMonsterIdleState::~PlantMonsterIdleState()
{

}

fq::client::PlantMonsterIdleState::PlantMonsterIdleState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterIdleState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterIdleState>(*this);
}

void fq::client::PlantMonsterIdleState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<PlantMonster>();
	monster->FindTarget();
}
