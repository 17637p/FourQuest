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
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());

	// 몬스터 죽음 이벤트 발생
	auto explosion = animator.GetComponent<MeleeMonsterExplosion>();
	if (explosion)
	{
		scene->GetEventManager()->FireEvent<client::event::KillMonster>(
			{ EMonsterType::Explosion });
	}
	else
	{
		scene->GetEventManager()->FireEvent<client::event::KillMonster>(
			{ EMonsterType::Melee });
	}
}

void fq::client::MeleeMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mEraseTime = 10.f;

	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	auto gameObject = animator.GetGameObject();

	gameObject->RemoveComponent<game_module::CapsuleCollider>();
	gameObject->RemoveComponent<game_module::NavigationAgent>();
	gameObject->RemoveComponent<game_module::ImageUI>();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Death", false , 0 });

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

void fq::client::MeleeMonsterDeadState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mDurationTime += dt;

	if (mDurationTime >= mEraseTime)
	{
		auto scene = animator.GetScene();
		scene->DestroyGameObject(animator.GetGameObject());

		// 몬스터 죽음 이벤트 발생
		auto explosion = animator.GetComponent<MeleeMonsterExplosion>();
		if (explosion)
		{
			scene->GetEventManager()->FireEvent<client::event::KillMonster>(
				{ EMonsterType::Explosion });
		}
		else
		{
			scene->GetEventManager()->FireEvent<client::event::KillMonster>(
				{ EMonsterType::Melee });
		}
	}
}

