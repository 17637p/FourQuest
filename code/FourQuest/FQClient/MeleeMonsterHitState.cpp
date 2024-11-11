#include "MeleeMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

fq::client::MeleeMonsterHitState::MeleeMonsterHitState()
	: mElapsed(0.f)
	, mDuration(0.1f)
	, mHitColor(1, 0, 0, 1)
	, mRimPow(1.f)
	, mRimIntensity(1.f)
{

}

fq::client::MeleeMonsterHitState::~MeleeMonsterHitState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterHitState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterHitState>(*this);
}

void fq::client::MeleeMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsed = 0.f;

	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info = skeletalMesh->GetMaterialInstanceInfo();
			info.bUseRimLight = true;
			info.RimLightColor = mHitColor;
			info.RimPow = mRimPow;
			info.RimIntensity = mRimIntensity;
			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}

void fq::client::MeleeMonsterHitState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsed += dt;

	if (mDuration < mElapsed)
	{
		for (auto child : animator.GetGameObject()->GetChildren())
		{
			auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

			if (skeletalMesh != nullptr)
			{
				fq::graphics::MaterialInstanceInfo info = skeletalMesh->GetMaterialInstanceInfo();
				info.bUseRimLight = false;
				skeletalMesh->SetMaterialInstanceInfo(info);
			}
		}
	}
}

void fq::client::MeleeMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info = skeletalMesh->GetMaterialInstanceInfo();
			info.bUseRimLight = false;
			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}
