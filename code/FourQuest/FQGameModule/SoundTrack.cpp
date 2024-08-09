#include "SoundTrack.h"

namespace fq::game_module
{
	SoundTrack::SoundTrack()
		: Track(ETrackType::SOUND)
	{
	}

	SoundTrack::~SoundTrack()
	{
	}

	void SoundTrack::Initialize(const SoundTrackInfo& info, Scene* scene)
	{
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mKeyName = info.keyName;
		mSoundPath = info.soundPath;
		mbIsLoop = info.bIsLoop;
	}
	
	void SoundTrack::PlayEnter()
	{
	}
	
	void SoundTrack::PlayOn()
	{
	}
	
	void SoundTrack::PlayExit()
	{
	}

	void SoundTrack::End()
	{
	}
}
