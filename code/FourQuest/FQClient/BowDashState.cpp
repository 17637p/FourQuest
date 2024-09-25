#include "BowDashState.h"

#include "ArcherArmour.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	BowDashState::BowDashState()
		: mMaxSpeedMultiplier()
		, mMinSpeedMultiplier()
		, mCurrentSpeedMultiplier()
		, mRotationSpeed()
		, mPeakSpeedTime()
		, mDurationTime()
		, mSpeedDecayRate()
		, mDashEffect(nullptr)
	{
	}
	BowDashState::~BowDashState()
	{
	}

	void BowDashState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		mDurationTime = 0.f;

		auto archerArmour = animator.GetComponent<ArcherArmour>();
		auto controller = animator.GetComponent<game_module::CharacterController>();

		mDashEffect = archerArmour->EmitDash();

		// 플레이어 태그를 변경
		animator.GetGameObject()->SetTag(game_module::ETag::Dash);
	}

	void BowDashState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mDurationTime += dt;

		auto controller = animator.GetComponent<game_module::CharacterController>();
		auto transform = animator.GetComponent<game_module::Transform>();
		auto rigidBody = animator.GetComponent<game_module::RigidBody>();
		auto archer = animator.GetComponent<ArcherArmour>();
		archer->SetLookAtLStickInput(dt, mRotationSpeed);

		if (mDurationTime <= mPeakSpeedTime)
		{
			mCurrentSpeedMultiplier = std::lerp(1.f, mMaxSpeedMultiplier, mDurationTime / mPeakSpeedTime);
		}
		else
		{
			mCurrentSpeedMultiplier = std::fmax(mMinSpeedMultiplier, mCurrentSpeedMultiplier - mSpeedDecayRate * dt);
		}

		DirectX::SimpleMath::Vector3 direction = transform->GetWorldMatrix().Forward();
		rigidBody->SetLinearVelocity(direction * (mCurrentSpeedMultiplier * controller->GetBaseSpeed()));
		controller->AddFinalSpeedMultiplier(mCurrentSpeedMultiplier - 1.f);
	}

	void BowDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		if (mDashEffect != nullptr)
		{
			animator.GetScene()->DestroyGameObject(mDashEffect.get());
			mDashEffect = nullptr;
		}

		animator.GetGameObject()->SetTag(game_module::ETag::Player);
	}


	std::shared_ptr<fq::game_module::IStateBehaviour> BowDashState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowDashState>(*this);
	}
	entt::meta_handle BowDashState::GetHandle()
	{
		return *this;
	}
}
