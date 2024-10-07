#include "SpawnerDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
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


void fq::client::SpawnerDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// ¸ó½ºÅÍ Á×À½ ÀÌº¥Æ® 
	auto scene = animator.GetScene();
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Spawner });

	// RimLight ²ô±â
	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = false;

			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}

void fq::client::SpawnerDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<MonsterSpawner>()->Destroy();

	// Á×¾ú´Âµ¥ ArmourSpawner ÄÄÆ÷³ÍÆ®°¡ ÀÖÀ» °æ¿ì °©¿Ê ¼ÒÈ¯
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}
