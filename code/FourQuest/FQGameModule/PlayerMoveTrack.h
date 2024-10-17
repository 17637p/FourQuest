#pragma once

#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class PlayerMoveTrack : public Track
	{
	public:
		PlayerMoveTrack();
		~PlayerMoveTrack();

		bool Initialize(const PlayerMoveTrackInfo& info, Scene* scene);

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		Scene* mScene;
		std::weak_ptr<GameObject>		mTargetObject;
		std::vector<TrackKey>			mKeys;

		unsigned int					mPlayerID;
		bool							mbIsObjectReturnToStartTransform;
		DirectX::SimpleMath::Vector3	mPrevPosition;
		DirectX::SimpleMath::Quaternion mPrevRotation;
		DirectX::SimpleMath::Vector3	mPrevScale;
	};
}