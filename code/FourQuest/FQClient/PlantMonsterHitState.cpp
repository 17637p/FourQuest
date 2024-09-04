#include "PlantMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

namespace fq::client
{
	PlantMonsterHitState::PlantMonsterHitState()
		: mDurationTime()
		, mRimTotalTime()
	{
	}
	PlantMonsterHitState::~PlantMonsterHitState()
	{
	}
	void PlantMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		mDurationTime = 0.f;
		mRimTotalTime = 0.15f;

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

	void PlantMonsterHitState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
	{
		mDurationTime += dt;

		if (mDurationTime >= mRimTotalTime)
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

	void PlantMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
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

	std::shared_ptr<fq::game_module::IStateBehaviour> PlantMonsterHitState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterHitState>(*this);
	}
}
