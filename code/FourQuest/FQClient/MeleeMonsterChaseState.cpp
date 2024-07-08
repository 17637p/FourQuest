#include "MeleeMonsterChaseState.h"

#include "../FQGameModule/GameModule.h"

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
