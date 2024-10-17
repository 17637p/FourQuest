#include "VibrationState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::VibrationState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<VibrationState>(*this);
}

void fq::client::VibrationState::OnStateEnter(fq::game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 컨트롤러 ID 연결 
	if (!mbUseAllController && animator.GetComponent<game_module::CharacterController>())
	{
		mControllerID = animator.GetComponent<game_module::CharacterController>()->GetControllerID();
	}

	mElapsedTime = 0.f;

	if (mVibrationTime == 0.f)
	{
		auto input = animator.GetScene()->GetInputManager();

		if (mbUseAllController)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; ++i)
			{
				input->SetVibration(i, mMode, mIntensity, mDuration);
			}
		}
		else if (mControllerID != NoID)
		{
			input->SetVibration(mControllerID, mMode, mIntensity, mDuration);
		}
	}
}

void fq::client::VibrationState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mVibrationTime == 0.f) return;

	mElapsedTime += dt;

	if (mElapsedTime >= mVibrationTime)
	{
		mVibrationTime = 0.f;
		
		auto input = animator.GetScene()->GetInputManager();

		if (mbUseAllController)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; ++i)
			{
				input->SetVibration(i, mMode, mIntensity, mDuration);
			}
		}
		else if (mControllerID != NoID)
		{
			input->SetVibration(mControllerID, mMode, mIntensity, mDuration);
		}
	}
}

void fq::client::VibrationState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	if (mbOffVibrationExitState)
	{
		auto input = animator.GetScene()->GetInputManager();

		if (mbUseAllController)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; ++i)
			{
				input->SetVibration(i, mMode, 0.f, 0.f);
			}
		}
		else if (mControllerID != NoID)
		{
			input->SetVibration(mControllerID, mMode, 0.f, 0.f);
		}
	}
}

fq::client::VibrationState::VibrationState()
	:mIntensity(0.f)
	, mElapsedTime(0.f)
	, mMode(EVibrationMode::Both)
	, mDuration(0.f) 
	, mbUseAllController(false)
	, mbOffVibrationExitState(false)
	, mVibrationTime(0.f)
	, mControllerID(NoID)
{}
