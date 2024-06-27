#include "PlayerDeadState.h"

#include "../FQGameModule/GameModule.h"


std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlayerDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlayerDeadState>(*this);
}

fq::client::PlayerDeadState::PlayerDeadState()
{

}

fq::client::PlayerDeadState::~PlayerDeadState()
{

}
