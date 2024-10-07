#include "VibrationTrack.h"

#include "InputManager.h"
#include "Scene.h"

namespace fq::game_module
{
	VibrationTrack::VibrationTrack()
		: Track(ETrackType::VIBRATION)
		, mScene(nullptr)
		, mVibrationMode()
		, mIntensity()
	{
	}
	VibrationTrack::~VibrationTrack()
	{
	}

	bool VibrationTrack::Initialize(const VibrationTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mVibrationMode = info.mode;
		mIntensity = info.Intensity;

		// 강도 계산                                                                                                                                                                                                                     
		mIntensity = std::clamp(mIntensity, 0.f, 100.f);
		mIntensity = mIntensity / 100.f * USHRT_MAX;

		return true;
	}


	void VibrationTrack::PlayEnter()
	{
		auto input = mScene->GetInputManager();

		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			input->SetVibration(i, mVibrationMode, mIntensity, mTotalPlayTime);
		}
	}

	void VibrationTrack::PlayOn()
	{

	}

	void VibrationTrack::PlayExit()
	{
		auto input = mScene->GetInputManager();

		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			input->SetVibration(i, EVibrationMode::Both, 0.f, mTotalPlayTime);
		}
	}

	void VibrationTrack::End()
	{
		auto input = mScene->GetInputManager();

		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			input->SetVibration(i, EVibrationMode::Both, 0.f, mTotalPlayTime);
		}
	}
}
