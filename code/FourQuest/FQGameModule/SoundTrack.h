#pragma once

#include "Track.h"
#include "Sequence.h"

namespace fq::game_module
{
	class SoundTrack : public Track
	{
	public:
		SoundTrack();
		~SoundTrack();

		void Initialize(const SoundTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		std::string mKeyName;
		std::string mSoundPath;
		bool mbIsLoop = false;
	};
}

