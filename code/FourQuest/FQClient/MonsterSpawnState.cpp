#include "MonsterSpawnState.h"

#include "../FQGameModule/GameModule.h"
#include "PlantMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterSpawnState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterSpawnState>(*this);

}

void fq::client::MonsterSpawnState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.SetParameterBoolean("OnSpawnEvent", false);
	animator.SetParameterBoolean("OnSpawn", false);
}

fq::client::MonsterSpawnState::MonsterSpawnState()
{}

fq::client::MonsterSpawnState::~MonsterSpawnState()
{}

