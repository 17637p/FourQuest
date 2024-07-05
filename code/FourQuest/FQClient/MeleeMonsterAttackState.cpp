#include "MeleeMonsterAttackState.h"

#include "../FQGameModule/GameModule.h"

fq::client::MeleeMonsterAttackState::MeleeMonsterAttackState()
{

}

fq::client::MeleeMonsterAttackState::~MeleeMonsterAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterAttackState>(*this);
}
