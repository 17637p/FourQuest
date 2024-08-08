#pragma once

#include "Sequence.h"
#include "Track.h"

namespace fq::game_module
{
	class EffectTrack : public Track
	{
	public:
		EffectTrack();
		~EffectTrack();

		void Initialize(EffectTrackInfo info, Scene* scene);

	private:
		virtual void End() override;
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;

	private:
		GameObject* mEffectObject;
	};
}

