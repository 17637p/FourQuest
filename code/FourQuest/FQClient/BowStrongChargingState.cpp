#include "BowStrongChargingState.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/InputManager.h"

#include "ArcherArmour.h"

namespace fq::client
{
	BowStrongChargingState::BowStrongChargingState()
		: mChargingElapsedTime()
		, mRotationSpeed()
		, mStringAttackIndex(1u)
		, mForcedChargingWaitingTime(0.f)
		, mbIsEmitAttack(false)
	{
	}

	BowStrongChargingState::~BowStrongChargingState()
	{
	}

	void BowStrongChargingState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		mChargingElapsedTime = 0.f;
		auto archer = animator.GetComponent<ArcherArmour>();
		mbIsEmitAttack = false;
	}

	void BowStrongChargingState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mChargingElapsedTime += dt;

		auto archer = animator.GetComponent<ArcherArmour>();
		archer->SetLookAtLStickInput(dt, mRotationSpeed);

		auto controller = animator.GetComponent<game_module::CharacterController>();
		auto controllerID = controller->GetControllerID();
		auto input = animator.GetGameObject()->GetScene()->GetInputManager();

		// 무조건 0.5초 당기고 차징한 시간에 따라 공격이 달라지도록
		if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::None) && mChargingElapsedTime >= mForcedChargingWaitingTime)
		{
			animator.SetParameterBoolean("OnCharging", false);

			if (!mbIsEmitAttack)
			{
				archer->EmitStrongAttack(mStringAttackIndex);
				mbIsEmitAttack = true;
			}
		}
		else
		{
			animator.SetParameterBoolean("OnCharging", true);
		}

		if (mChargingWationTime < mChargingElapsedTime)
		{
			animator.SetParameterBoolean("OnNextStay", true);
		}
	}

	void BowStrongChargingState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		animator.SetParameterBoolean("OnNextStay", false);
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BowStrongChargingState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowStrongChargingState>(*this);
	}
	entt::meta_handle BowStrongChargingState::GetHandle()
	{
		return *this;
	}
}
