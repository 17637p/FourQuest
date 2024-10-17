#pragma once

#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class PlayerTeleportTrack : public Track
	{
	public:
		PlayerTeleportTrack();
		~PlayerTeleportTrack();

		bool Initialize(const PlayerTeleportTrackInfo& info, Scene* scene);

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		std::weak_ptr<GameObject>		mTargetObject;
		std::vector<TrackKey>			mKeys;

		unsigned int					mPlayerID;
		bool							mbIsObjectReturnToStartTransform;
		DirectX::SimpleMath::Vector3	mPrevPosition;
		DirectX::SimpleMath::Quaternion mPrevRotation;
		DirectX::SimpleMath::Vector3	mPrevScale;
	};

}