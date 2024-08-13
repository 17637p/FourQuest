#pragma once

#include "Sequence.h"
#include "Track.h"

namespace fq::game_module
{
	class ObjectTeleportTrack : public Track
	{
	public:
		ObjectTeleportTrack();
		~ObjectTeleportTrack();

		bool Initialize(const ObjectTeleportTrackInfo& info, Scene* scene);

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

		std::weak_ptr<GameObject> GetTargetObject() { return mTargetObject; }
		const DirectX::SimpleMath::Vector3& GetPrevPosition() { return mPrevPosition; }
		const DirectX::SimpleMath::Quaternion& GetPrevRotation() { return mPrevRotation; }
		const DirectX::SimpleMath::Vector3& GetPrevPScale() { return mPrevScale; }
		const std::vector<TrackKey>& GetTargetPosition() { return mKeys; }

		void SetTargetObject(std::weak_ptr<GameObject> object) { mTargetObject = object; }
		void SetPrevPosition(const DirectX::SimpleMath::Vector3& position) { mPrevPosition = position; }
		void SetPrevRotation(const DirectX::SimpleMath::Quaternion& rotation) { mPrevRotation = rotation; }
		void SetPrevScale(const DirectX::SimpleMath::Vector3& scale) { mPrevScale = scale; }
		void SetTargetPosition(const std::vector<TrackKey>& keys) { mKeys = keys; }

	private:
		std::weak_ptr<GameObject> mTargetObject;
		std::vector<TrackKey> mKeys;

		DirectX::SimpleMath::Vector3 mPrevPosition;
		DirectX::SimpleMath::Quaternion mPrevRotation;
		DirectX::SimpleMath::Vector3 mPrevScale;
	};
}