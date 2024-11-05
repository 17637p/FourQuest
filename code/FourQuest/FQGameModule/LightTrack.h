#pragma once
#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class LightTrack : public Track
	{
	public:
		LightTrack();
		~LightTrack();

		bool Initialize(const LightTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		Scene* mScene;

		std::string mTargetObjectName;
		std::shared_ptr<GameObject> mTargetObject;

		float mPrevIntensity;
		std::vector<LightTrackKey> mLightTrackKeys;
	};

}