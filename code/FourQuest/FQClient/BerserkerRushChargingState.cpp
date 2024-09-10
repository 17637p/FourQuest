#include "BerserkerRushChargingState.h"
#include "BerserkerArmour.h"
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
	{
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BerserkerRushChargingState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BerserkerRushChargingState>(*this);
	}

	void BerserkerRushChargingState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		// 에디터에서 이펙트 생성

		// 우측 키보드로 입력을 받음
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotation(fq::game_module::EPadStickType::Right);
		controller->SetDashInput(true);

		mChargingElapsedTime = 0.f;
		mbPassedPoint = false;
	}

	void BerserkerRushChargingState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
	{
		auto input = animator.GetScene()->GetInputManager();
		auto controller = animator.GetComponent<game_module::CharacterController>();
		mChargingElapsedTime = std::min<float>(mChargingElapsedTime + dt * state.GetPlayBackSpeed(), mChargingTime);
		controller->SetPadInputRotation(fq::game_module::EPadStickType::Right);

		if (!input->IsPadKeyState(controller->GetControllerID(), EPadKey::RightThumb, EKeyState::Hold))
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
	}
}