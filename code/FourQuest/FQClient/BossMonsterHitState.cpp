#include "BossMonsterHitState.h"
#include "BossMonster.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

fq::client::BossMonsterHitState::BossMonsterHitState()
	: mElapsed(0.f)
	, mDuration(0.1f)
	, mHitColor(1, 0, 0, 1)
	, mRimPow(1.f)
	, mRimIntensity(1.f)
{
}

fq::client::BossMonsterHitState::~BossMonsterHitState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterHitState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterHitState>(*this);
}

void fq::client::BossMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });

	mElapsed = 0.f;

	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = true;
			info.RimLightColor = mHitColor;
			info.RimPow = mRimPow;
			info.RimIntensity = mRimIntensity;

			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}

void fq::client::BossMonsterHitState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsed += dt;

	if (mDuration < mElapsed)
	{
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
}

void fq::client::BossMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
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

