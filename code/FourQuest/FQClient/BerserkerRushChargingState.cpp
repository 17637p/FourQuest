#include "BerserkerRushChargingState.h"
#include "BerserkerArmour.h"
#include "Player.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/AnimatorController.h"
#include "../FQGameModule/InputEnum.h"
#include "../FQGameModule/InputManager.h"

namespace fq::client
{
	BerserkerRushChargingState::BerserkerRushChargingState()
		: mChargingTime(1.f)
		, mChargingMinimumTime(0.5f)
		, mChargingElapsedTime(0.f)
		, mbPassedPoint(false)
		, mRotationSpeed(1.f)
	{
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BerserkerRushChargingState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BerserkerRushChargingState>(*this);
	}

	void BerserkerRushChargingState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotation(fq::game_module::EPadStickType::Right);
		controller->SetDashInput(true);

		mChargingElapsedTime = 0.f;
		mbPassedPoint = false;

		auto playerOrNull = animator.GetComponent<Player>();
		if (playerOrNull != nullptr)
		{
			playerOrNull->SetIsActiveOnHit(false);
		}

		auto berserkerArmour = animator.GetComponent<BerserkerArmour>();
		if (berserkerArmour != nullptr)
		{
			berserkerArmour->EmitSound(EBerserkerSoundType::RushReady);
		}
	}

	void BerserkerRushChargingState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
	{
		auto input = animator.GetScene()->GetInputManager();
		auto controller = animator.GetComponent<game_module::CharacterController>();
		auto transform = animator.GetComponent<game_module::Transform>();
		mChargingElapsedTime = std::min<float>(mChargingElapsedTime + dt * state.GetPlayBackSpeed(), mChargingTime);

		DirectX::SimpleMath::Vector3 rightInput{};
		rightInput.x = input->GetStickInfomation(controller->GetControllerID(), EPadStick::rightX);
		rightInput.z = input->GetStickInfomation(controller->GetControllerID(), EPadStick::rightY);
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		if (rightInput.LengthSquared() >= rotationOffsetSq)
		{
			rightInput.Normalize();
			float targetYaw = atan2f(-rightInput.x, -rightInput.z); // atan2 사용으로 XZ 평면 기준 각도 계산

			DirectX::SimpleMath::Quaternion inputRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(targetYaw, 0, 0);
			DirectX::SimpleMath::Quaternion startRotation = transform->GetWorldRotation();
			DirectX::SimpleMath::Quaternion newRotation = DirectX::SimpleMath::Quaternion::Slerp(startRotation, inputRotation, mRotationSpeed * dt);

			transform->SetWorldRotation(newRotation);
		}
		else
		{
			if (mChargingElapsedTime < mChargingMinimumTime)
			{
				animator.SetParameterTrigger("OnIdle");
			}
			else if (mChargingElapsedTime == mChargingTime)
			{
				animator.SetParameterTrigger("OnRush");
			}
		}
	}

	void BerserkerRushChargingState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetDashInput(false);

		auto playerOrNull = animator.GetComponent<Player>();
		if (playerOrNull != nullptr)
		{
			playerOrNull->SetIsActiveOnHit(true);
		}
	}
}