#pragma once

#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class FadeTrack : public Track
	{
	public:
		FadeTrack();
		~FadeTrack();

		bool Initialize(const FadeTrackInfo& info, Scene* scene);

	private:
		virtual void End() override;
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;

	private:
		Scene* mScene;
		std::shared_ptr<GameObject> mPostProcessObject;

		std::string mPostProcessObjectName;
		float mBlendTime;
		DirectX::SimpleMath::Color mPrevColor;
		DirectX::SimpleMath::Color mFinalColor;
	};
}