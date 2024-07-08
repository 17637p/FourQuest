#include "MeleeMonsterIdleState.h"

#include "../FQGameModule/GameModule.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterIdleState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterIdleState>(*this);
}

fq::client::MeleeMonsterIdleState::MeleeMonsterIdleState()
{

}

fq::client::MeleeMonsterIdleState::~MeleeMonsterIdleState()
{

}
