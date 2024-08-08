#include "Track.h"

namespace fq::game_module
{
	Track::Track(ETrackType type)
		: mType(type)
		, mCurrentState(ETrackState::OFF)
		, mStartTime(0.f)
		, mTotalPlayTime(0.f)
		, mElapsedTime(0.f)
	{
	}

	ETrackState Track::Play(float durationTime)
	{
		mElapsedTime = durationTime - mStartTime;

		if (mCurrentState == ETrackState::END || mElapsedTime < 0.f)
			return ETrackState::OFF;

		handleState();
		nodeUpdate();

		return mCurrentState;
	}

	void Track::WakeUp()
	{
		mCurrentState = fq::game_module::ETrackState::OFF;
	}

	void Track::handleState()
	{
		switch (mCurrentState)
		{
		case fq::game_module::ETrackState::OFF:
			mCurrentState = ETrackState::ENTER;
			break;
		case fq::game_module::ETrackState::ENTER:
			mCurrentState = ETrackState::ON;
			break;
		case fq::game_module::ETrackState::ON:
			if (mTotalPlayTime <= mElapsedTime)
			{
				mCurrentState = ETrackState::EXIT;
			}
			break;
		case fq::game_module::ETrackState::EXIT:
			mCurrentState = ETrackState::END;
			break;
		default:
			assert(false);
			break;
		}
	}
	void Track::nodeUpdate()
	{
		switch (mCurrentState)
		{
		case fq::game_module::ETrackState::ENTER:
			PlayEnter();
			break;
		case fq::game_module::ETrackState::ON:
			PlayOn();
			break;
		case fq::game_module::ETrackState::EXIT:
			PlayExit();
			break;
		}
	}
}
