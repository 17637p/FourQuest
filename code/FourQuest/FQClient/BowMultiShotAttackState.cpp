#include "BowMultiShotAttackState.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/InputManager.h"
#include "ArcherArmour.h"
#include "PlayerVariable.h"

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
		auto controller = animator.GetComponent<fq::game_module::CharacterController>();

		controller->SetPadInputRotationBySpeed(game_module::EPadStickType::Right, PlayerVariable::PadRotationSpeed ,dt);
		archer->AimToNearMonster();

		controller->AddFinalSpeedMultiplier((-controller->GetFinalSpeedMultiplier() / 2.f));

		mShotElapsedTime += dt;

		if (mShotElapsedTime >= mShotDelay)
		{
			mShotElapsedTime -= mShotDelay;

			archer->EmitmWeakAttack();

			size_t index = (size_t)EArcherSound::Fastshoot1 + rand() % 3;
			archer->EmitSound((EArcherSound)index);
		}

		archer->CheckPreXInput();
	}
	void BowMultiShotAttackState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
	{
		animator.SetParameterBoolean("OnPreX", false);
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
