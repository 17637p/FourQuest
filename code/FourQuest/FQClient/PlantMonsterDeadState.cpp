#include "PlantMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/MaterialAnimator.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/ImageUI.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

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
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());

	// 몬스터 죽음 이벤트
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Plant });
}

void fq::client::PlantMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mEraseTime = 10.f;

	auto gameObject = animator.GetGameObject();
	gameObject->RemoveComponent<game_module::CapsuleCollider>();
	gameObject->RemoveComponent<game_module::ImageUI>();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MR_Death", false , 3 });

	// Ragdoll
	if (animator.GetGameObject()->HasComponent<game_module::Articulation>())
	{
		auto articulation = animator.GetComponent<game_module::Articulation>();

		articulation->SetIsRagdoll(true);
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
			info.DelayTime = mEraseTime - 1.f;
			matAnimator->SetDissolveAnimatorInfo(info);
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

		// 몬스터 죽음 이벤트
		scene->GetEventManager()->FireEvent<client::event::KillMonster>(
			{ EMonsterType::Plant });
	}
}
