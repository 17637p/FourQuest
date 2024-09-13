#include "MeleeMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

fq::client::MeleeMonsterHitState::MeleeMonsterHitState()
	: mDurationTime()
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
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	for (auto child : animator.GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = true;
			info.RimLightColor = DirectX::SimpleMath::Color{ 1.f, 0.f, 0.f, 1.f };
			info.RimPow = 0.f;
			info.RimIntensity = 1.f;
			info.bUseInvRimLight = true;
			info.InvRimLightColor = DirectX::SimpleMath::Color{ 1.f, 0.f, 0.f, 1.f };
			info.InvRimPow = 0.f;
			info.InvRimIntensity = 1.f;

			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}

void fq::client::MeleeMonsterHitState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mDurationTime += dt;

	if (mDurationTime >= 0.15f)
	{
		mDurationTime = 0.f;

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

void fq::client::MeleeMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mDurationTime = 0.f;

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
