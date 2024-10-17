#include "MeleeMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQClient/HpBar.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/MaterialAnimator.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "MeleeMonsterExplosion.h"

#include "ArmourSpawner.h"
#include "MonsterVariable.h"

fq::client::MeleeMonsterDeadState::MeleeMonsterDeadState()
	: mEraseTime()
{

}

fq::client::MeleeMonsterDeadState::~MeleeMonsterDeadState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterDeadState>(*this);

}

void fq::client::MeleeMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// �׾��µ� ArmourSpawner ������Ʈ�� ���� ��� ���� ��ȯ
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}

void fq::client::MeleeMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	auto gameObject = animator.GetGameObject();

	gameObject->RemoveComponent<game_module::CapsuleCollider>();
	gameObject->RemoveComponent<game_module::ImageUI>();
	gameObject->RemoveComponent<HpBar>();

	// ���� ���
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Death", false , fq::sound::EChannel::SE });

	// ���� ���� �̺�Ʈ �߻�
	auto scene = animator.GetScene();
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Melee });

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

	// RimLight ����
	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = false;
			skeletalMesh->SetMaterialInstanceInfo(info);
			auto objectInfo = skeletalMesh->GetMeshObjectInfomation();
			objectInfo.OutlineColor = { 0.f,0.f,0.f,1.f };
			objectInfo.bUseShadow = false;
			skeletalMesh->SetMeshObjectInfomation(objectInfo);
		}
	}
}

void fq::client::MeleeMonsterDeadState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mDurationTime += dt;

	if (mDurationTime >= mEraseTime)
	{
		auto scene = animator.GetScene();
		scene->DestroyGameObject(animator.GetGameObject());
	}
}

