#pragma once

#include "Sequence.h"
#include "Track.h"

namespace fq::game_module
{
	class ObjectAnimationTrack : public Track
	{
	public:
		ObjectAnimationTrack();
		~ObjectAnimationTrack();

		void Initialize(ObjectAnimationInfo info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:

	};
}


