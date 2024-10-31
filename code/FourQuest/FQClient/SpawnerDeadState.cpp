#include "SpawnerDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "MonsterSpawner.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/MaterialAnimator.h"


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
	// ���� ���� �̺�Ʈ 
	auto scene = animator.GetScene();
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Spawner });

	// RimLight ����
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

	// BurnEffect
	for (auto child : animator.GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::MaterialAnimator>())
		{
			auto matAnimator = child->GetComponent<game_module::MaterialAnimator>();
			auto info = matAnimator->GetDissolveAnimatorInfo();
			info.bIsUpdate = true;
			info.bIsUsed = true;
			info.Duration = 2.f;
			info.Speed = 2.f;
			info.DelayTime = 0.f;
			info.MinDissolveCutoff = -1.f;
			info.InitDissolveCutoff = -1.f;
			matAnimator->SetDissolveAnimatorInfo(info);
		}
	}
}

void fq::client::SpawnerDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<MonsterSpawner>()->Destroy();

	// �׾��µ� ArmourSpawner ������Ʈ�� ���� ��� ���� ��ȯ
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}
