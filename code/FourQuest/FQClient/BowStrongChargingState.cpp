#include "BowStrongChargingState.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/InputManager.h"

#include "ArcherArmour.h"
#include "PlayerVariable.h"

namespace fq::client
{
	BowStrongChargingState::BowStrongChargingState()
		: mChargingElapsedTime()
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
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotation(game_module::EPadStickType::Left);

		mChargingElapsedTime = 0.f;
		auto archer = animator.GetComponent<ArcherArmour>();
		mbIsEmitAttack = false;

		if (archer != nullptr)
		{
			switch (mStringAttackIndex)
			{
				case 1:
					archer->EmitSound(EArcherSound::ShootStart);
					break;
				case 2:
					// fall through
				case 3:
					archer->EmitSound(EArcherSound::Charge1);
					break;
				case 4:
					archer->EmitSound(EArcherSound::Charge2);
					break;
				default:
					assert(false);
					break;
			}
		}
	}

	void BowStrongChargingState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mChargingElapsedTime += dt;

		auto archer = animator.GetComponent<ArcherArmour>();
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotationBySpeed(game_module::EPadStickType::Left, PlayerVariable::PadRotationSpeed, dt);

		auto controllerID = controller->GetControllerID();
		auto input = animator.GetGameObject()->GetScene()->GetInputManager();

		// 무조건 0.5초 당기고 차징한 시간에 따라 공격이 달라지도록
		if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::None) && mChargingElapsedTime >= mForcedChargingWaitingTime)
		{
			animator.SetParameterBoolean("OnCharging", false);

			if (!mbIsEmitAttack)
			{
				archer->RemoveLineOfSight();

				archer->EmitSound(EArcherSound::Shoot);
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
