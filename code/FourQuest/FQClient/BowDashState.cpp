#include "BowDashState.h"

#include "ArcherArmour.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

namespace fq::client
{
	BowDashState::BowDashState()
		: mOriginSpeed()
		, mMaxSpeed()
		, mMinSpeed()
		, mCurrentSpeed()
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
		mOriginSpeed = archerArmour->GetOriginCharacterMaxSpeed();

		// 플레이어 태그를 변경
		animator.GetGameObject()->SetTag(game_module::ETag::Dash);
	}

	void BowDashState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mDurationTime += dt;

		if (mDurationTime <= mPeakSpeedTime)
		{
			auto controller = animator.GetComponent<game_module::CharacterController>();

			mCurrentSpeed = std::lerp(mOriginSpeed, mMaxSpeed, mDurationTime / mPeakSpeedTime);
			
			auto movementInfo = controller->GetMovementInfo();
			movementInfo.maxSpeed = mCurrentSpeed;
			controller->SetMovementInfo(movementInfo);
		}
		else
		{
			auto controller = animator.GetComponent<game_module::CharacterController>();

			mCurrentSpeed = std::fmax(mMinSpeed, mCurrentSpeed - mSpeedDecayRate * dt);

			auto movementInfo = controller->GetMovementInfo();
			movementInfo.maxSpeed = mCurrentSpeed;
			controller->SetMovementInfo(movementInfo);
		}
	}

	void BowDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		auto controller = animator.GetComponent<game_module::CharacterController>();

		auto movementInfo = controller->GetMovementInfo();
		movementInfo.maxSpeed = mOriginSpeed;
		controller->SetMovementInfo(movementInfo);

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
