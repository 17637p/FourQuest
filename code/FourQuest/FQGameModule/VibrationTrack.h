#pragma once
#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class VibrationTrack : public Track
	{
	public:
		VibrationTrack();
		~VibrationTrack();

		bool Initialize(const VibrationTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		Scene* mScene;

		EVibrationMode mVibrationMode;
		float mIntensity;
	};
}

