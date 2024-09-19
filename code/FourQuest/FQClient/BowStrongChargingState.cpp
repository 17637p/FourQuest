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
	{
	}

	BowStrongChargingState::~BowStrongChargingState()
	{
	}

	void BowStrongChargingState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		mChargingElapsedTime = 0.f;
		auto archer = animator.GetComponent<ArcherArmour>();

		mChargingEffect = archer->EmitChargingEffect();
	}

	void BowStrongChargingState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		mChargingElapsedTime += dt;

		auto archer = animator.GetComponent<ArcherArmour>();
		archer->SetLookAtLStickInput(dt, mRotationSpeed);

		auto controller = animator.GetComponent<game_module::CharacterController>();
		auto controllerID = controller->GetControllerID();
		auto input = animator.GetGameObject()->GetScene()->GetInputManager();

		// ������ 0.5�� ���� ��¡�� �ð��� ���� ������ �޶�������
		if (input->IsPadKeyState(controllerID, EPadKey::X, EKeyState::None) && mChargingElapsedTime >= 0.5f)
		{
			animator.SetParameterBoolean("OnCharging", false);
			animator.SetParameterFloat("ChargingTime", mChargingElapsedTime);
		}
		else
		{
			animator.SetParameterBoolean("OnCharging", true);
		}
	}

	void BowStrongChargingState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		auto archer = animator.GetComponent<ArcherArmour>();
		float changeChargingTime = archer->GetChangeChargingTime();

		if (mChargingEffect != nullptr)
		{
			animator.GetScene()->DestroyGameObject(mChargingEffect.get());
			mChargingEffect = nullptr;
		}

		// ��¡ �ܰ迡 ���� �߻�Ǵ� ȭ�� ���Ⱑ �޶����ϴ�.
		if (changeChargingTime * 3 <= mChargingElapsedTime)
		{
			archer->EmitStrongAttack(4);
			archer->EmitStrongAttackEffect();
		}
		else if (changeChargingTime * 2 <= mChargingElapsedTime)
		{
			archer->EmitStrongAttack(3);
			archer->EmitStrongAttackEffect();
		}
		else if (changeChargingTime * 1 <= mChargingElapsedTime)
		{
			archer->EmitStrongAttack(2);
			archer->EmitStrongAttackEffect();
		}
		else
		{
			archer->EmitStrongAttack(1);
			archer->EmitStrongAttackEffect();
		}
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
