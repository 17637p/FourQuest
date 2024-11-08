#include "SoundTrack.h"

#include "Scene.h"
#include "SoundManager.h"

#include "Event.h"
#include "EventManager.h"

namespace fq::game_module
{
	unsigned int SoundTrack::SoundChannelIndexNumber = 16;

	SoundTrack::SoundTrack()
		: Track(ETrackType::SOUND)
		, mScene(nullptr)
		, mKeyName{}
		, mSoundObjectName{}
		, mbIsLoop()
		, mOnce(true)
	{
	}

	SoundTrack::~SoundTrack()
	{
	}

	bool SoundTrack::Initialize(const SoundTrackInfo& info, Scene* scene)
	{
		mScene = scene;
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mKeyName = info.keyName;
		mbIsLoop = info.bIsLoop;
		mSoundObjectName = info.soundObjectName;

		return true;
	}
	
	void SoundTrack::PlayEnter()
	{
		auto object = mScene->GetObjectByName(mSoundObjectName);

		if (object != nullptr)
		{
			if (!object->HasComponent<SoundClip>()) return;

			mMyChannelNumber = SoundChannelIndexNumber;
			auto soundClip = object->GetComponent<SoundClip>();
			soundClip->StopChannel(mMyChannelNumber);
			soundClip->Play(mKeyName, mbIsLoop, SoundChannelIndexNumber++);
			
			if (SoundChannelIndexNumber >= SoundManager::NoneStopChannel)
			{
				SoundChannelIndexNumber = 16;
			}
		}
		else
		{
			spdlog::warn("[Warrning] Scene do not have SoundClipObject");
		}
	}
	
	void SoundTrack::PlayOn()
	{
	}
	
	void SoundTrack::PlayExit()
	{
		auto object = mScene->GetObjectByName(mSoundObjectName);

		if (object != nullptr)
		{
			if (!object->HasComponent<SoundClip>()) return;

			auto soundClip = object->GetComponent<SoundClip>();
			soundClip->StopChannel(mMyChannelNumber);
		}
	}

	void SoundTrack::End()
	{
		auto object = mScene->GetObjectByName(mSoundObjectName);

		if (object != nullptr)
		{
			if (!object->HasComponent<SoundClip>()) return;

			auto soundClip = object->GetComponent<SoundClip>();
			soundClip->StopChannel(mMyChannelNumber);
		}
	}
}
