#include "BowStrongChargingState.h"

#include "../FQGameModule/Scene.h"
#include "ArcherArmour.h"

namespace fq::client
{
	BowStrongChargingState::BowStrongChargingState()
		: mChargingElapsedTime()
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
		archer->SetLookAtLStickInput();



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

		if (changeChargingTime <= mChargingElapsedTime)
		{
			archer->EmitStrongAttack(true);
		}
		else
		{
			archer->EmitStrongAttack(false);
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
