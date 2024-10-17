#include "SoundTrack.h"

#include "Scene.h"

namespace fq::game_module
{
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

			auto soundClip = object->GetComponent<SoundClip>();
			soundClip->Play(mKeyName, mbIsLoop, 3);
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
			soundClip->StopChannel(3);
			soundClip->OnDestroy();
		}
	}

	void SoundTrack::End()
	{
		auto object = mScene->GetObjectByName(mSoundObjectName);

		if (object != nullptr)
		{
			if (!object->HasComponent<SoundClip>()) return;

			auto soundClip = object->GetComponent<SoundClip>();
			soundClip->StopChannel(3);
			soundClip->OnDestroy();
		}
	}
}
