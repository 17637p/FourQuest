#include "SpawnerDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "MonsterSpawner.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/Scene.h"

#include "ArmourSpawner.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::SpawnerDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<SpawnerDeadState>(*this);
}

fq::client::SpawnerDeadState::SpawnerDeadState()
{}

fq::client::SpawnerDeadState::~SpawnerDeadState()
{}


void fq::client::SpawnerDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<MonsterSpawner>()->Destroy();

	// ���� ���� �̺�Ʈ 
	auto scene = animator.GetScene();
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Spawner });

	// �׾��µ� ArmourSpawner ������Ʈ�� ���� ��� ���� ��ȯ
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}
