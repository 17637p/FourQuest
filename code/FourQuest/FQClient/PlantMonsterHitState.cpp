#include "PlantMonsterHitState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"

namespace fq::client
{
	PlantMonsterHitState::PlantMonsterHitState()
		: mElapsed(0.f)
		, mDuration(0.1f)
		, mHitColor(1, 0, 0, 1)
		, mRimPow(1.f)
		, mRimIntensity(1.f)
	{
	}
	PlantMonsterHitState::~PlantMonsterHitState()
	{
	}
	void PlantMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
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

	void PlantMonsterHitState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
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

	void PlantMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
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

	std::shared_ptr<fq::game_module::IStateBehaviour> PlantMonsterHitState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterHitState>(*this);
	}
}
