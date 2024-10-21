#include "PlantMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/MaterialAnimator.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "ClientEvent.h"
#include "MonsterGroup.h"
#include "PlantMonster.h"

#include "ArmourSpawner.h"

fq::client::PlantMonsterDeadState::PlantMonsterDeadState()
{}

fq::client::PlantMonsterDeadState::~PlantMonsterDeadState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterDeadState>(*this);
}

void fq::client::PlantMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// Á×¾ú´Âµ¥ ArmourSpawner ÄÄÆ÷³ÍÆ®°¡ ÀÖÀ» °æ¿ì °©¿Ê ¼ÒÈ¯
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}

void fq::client::PlantMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto gameObject = animator.GetGameObject();

	gameObject->RemoveComponent<game_module::CapsuleCollider>();
	animator.GetComponent<PlantMonster>()->DestroyMonsterHPUI();


	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MR_Death", false , fq::sound::EChannel::SE });

	// ¸ó½ºÅÍ Á×À½ ÀÌº¥Æ®
	auto scene = animator.GetScene();
	auto monsterGroup = MonsterGroup::GetMonsterGroup(gameObject);
	if (monsterGroup)
	{
		monsterGroup->DestroyMonster(gameObject);
	}
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Plant });

	// Ragdoll
	if (animator.GetGameObject()->HasComponent<game_module::Articulation>())
	{
		auto articulation = animator.GetComponent<game_module::Articulation>();
		auto ragdoll = animator.GetComponent<game_module::RigidBody>();

		articulation->SetIsRagdoll(true);
		ragdoll->SetBodyType(game_module::RigidBody::EBodyType::Dynamic);
	}

	// BurnEffect
	for (auto child : gameObject->GetChildren())
	{
		if (child->HasComponent<game_module::MaterialAnimator>())
		{
			auto matAnimator = child->GetComponent<game_module::MaterialAnimator>();
			auto info = matAnimator->GetDissolveAnimatorInfo();
			info.bIsUpdate = true;
			info.bIsUsed = true;
			info.Duration = mEraseTime;
			info.DelayTime = mEraseTime - 2.f;
			info.MinDissolveCutoff = -1.f;
			info.InitDissolveCutoff = -1.f;
			matAnimator->SetDissolveAnimatorInfo(info);
		}
	}

	// RimLight , Outline ²ô±â
	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = false;
			skeletalMesh->SetMaterialInstanceInfo(info);
			auto objectInfo = skeletalMesh->GetMeshObjectInfomation();
			objectInfo.OutlineColor = {0.f,0.f,0.f,1.f};
			objectInfo.bUseShadow = false;
			skeletalMesh->SetMeshObjectInfomation(objectInfo);
		}
	}
}

void fq::client::PlantMonsterDeadState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mDurationTime += dt;

	if (mDurationTime >= mEraseTime)
	{
		auto scene = animator.GetScene();
		scene->DestroyGameObject(animator.GetGameObject());
	}
}
