#pragma once

#include "Track.h"
#include "SequenceStructure.h"

#include "SoundClip.h"

namespace fq::game_module
{
	class SoundTrack : public Track
	{
	public:
		SoundTrack();
		~SoundTrack();

		bool Initialize(const SoundTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		Scene* mScene;

		std::string mKeyName;
		std::string mSoundObjectName;
		bool mbIsLoop;
		bool mOnce;
	};
}

