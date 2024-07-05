#include "MeleeMonsterPatrolState.h"

#include "../FQGameModule/GameModule.h"


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
