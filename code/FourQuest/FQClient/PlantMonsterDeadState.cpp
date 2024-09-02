#include "PlantMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/MaterialAnimator.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

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
	auto gameObject = animator.GetGameObject();
	gameObject->RemoveComponent<game_module::CapsuleCollider>();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MR_Death", false , 3 });

	// BurnEffect
	for (auto child : gameObject->GetChildren())
	{
		if (child->HasComponent<game_module::MaterialAnimator>())
		{
			auto matAnimator = child->GetComponent<game_module::MaterialAnimator>();
			auto info = matAnimator->GetDissolveAnimatorInfo();
			info.bIsUpdate = true;
			info.bIsUsed = true;
			matAnimator->SetDissolveAnimatorInfo(info);
		}

		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			auto skinnedMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();
			auto info = skinnedMesh->GetMeshObjectInfomation();
			info.bUseShadow = false;
			skinnedMesh->SetMeshObjectInfomation(info);
		}
	}
}
