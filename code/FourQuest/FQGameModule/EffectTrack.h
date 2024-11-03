#pragma once

#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class EffectTrack : public Track
	{
	public:
		EffectTrack();
		~EffectTrack();

		bool Initialize(const EffectTrackInfo& info, Scene* scene);

	private:
		virtual void End() override;
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;

	private:
		Scene* mScene;
		std::shared_ptr<GameObject> mEffectObject;
		std::vector<std::shared_ptr<GameObject>> mObjects;

		std::string mPrefebPath;
		std::vector<TrackKey> mKeys;
	};
}

