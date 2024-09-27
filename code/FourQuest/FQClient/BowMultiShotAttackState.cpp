#include "BowMultiShotAttackState.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/InputManager.h"
#include "ArcherArmour.h"

namespace fq::client
{
	BowMultiShotAttackState::BowMultiShotAttackState()
		: mShotDelay()
		, mShotElapsedTime()
	{
	}
	BowMultiShotAttackState::~BowMultiShotAttackState()
	{
	}

	void BowMultiShotAttackState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}

	void BowMultiShotAttackState::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
	{
		auto archer = animator.GetComponent<ArcherArmour>();
		archer->SetLookAtRStickInput();
		archer->AimToNearMonster();

		auto controller = animator.GetComponent<fq::game_module::CharacterController>();
		controller->AddFinalSpeedMultiplier((-controller->GetFinalSpeedMultiplier() / 2.f));

		mShotElapsedTime += dt;

		if (mShotElapsedTime >= mShotDelay)
		{
			mShotElapsedTime -= mShotDelay;

			archer->EmitmWeakAttack();
		}
	}
	void BowMultiShotAttackState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
	}



	std::shared_ptr<fq::game_module::IStateBehaviour> BowMultiShotAttackState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BowMultiShotAttackState>(*this);
	}
	entt::meta_handle BowMultiShotAttackState::GetHandle()
	{
		return *this;
	}
}
