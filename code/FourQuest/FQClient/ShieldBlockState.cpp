#include "ShieldBlockState.h"

#include "../FQGameModule/GameModule.h"

fq::client::ShieldBlockState::ShieldBlockState()
{

}

fq::client::ShieldBlockState::~ShieldBlockState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::ShieldBlockState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<ShieldBlockState>(*this);
}
