#pragma once

#include "Track.h"

namespace fq::game_module
{
	class ObjectTeleportTrack : public Track
	{
	public:
		ObjectTeleportTrack(ETrackType type);
		~ObjectTeleportTrack();

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;


		std::weak_ptr<GameObject> GetGameObject() { return mTargetObject; }
		const DirectX::SimpleMath::Vector3& GetPrevPosition() { return mPrevPosition; }
		const DirectX::SimpleMath::Quaternion& GetPrevRotation() { return mPrevRotation; }
		const DirectX::SimpleMath::Vector3& GetPrevPScale() { return mPrevScale; }
		const DirectX::SimpleMath::Vector3& GetTargetPosition() { return mTargetPosition; }
		const DirectX::SimpleMath::Quaternion& GetTargetRotation() { return mTargetRotation; }
		const DirectX::SimpleMath::Vector3& GetPTargetScale() { return mTargetScale; }

		void SetTargetObject(std::weak_ptr<GameObject> object) { mTargetObject = object; }
		void SetPrevPosition(const DirectX::SimpleMath::Vector3& position) { mPrevPosition = position; }
		void SetPrevRotation(const DirectX::SimpleMath::Quaternion& rotation) { mPrevRotation = rotation; }
		void SetPrevScale(const DirectX::SimpleMath::Vector3& scale) { mPrevScale = scale; }
		void SetTargetPosition(const DirectX::SimpleMath::Vector3& position) { mTargetPosition = position; }
		void SetTargetRotation(const DirectX::SimpleMath::Quaternion& rotation) { mTargetRotation = rotation; }
		void SetTargetScale(const DirectX::SimpleMath::Vector3& scale) { mTargetScale = scale; }

	private:
		std::weak_ptr<GameObject> mTargetObject;

		DirectX::SimpleMath::Vector3 mPrevPosition;
		DirectX::SimpleMath::Quaternion mPrevRotation;
		DirectX::SimpleMath::Vector3 mPrevScale;
		DirectX::SimpleMath::Vector3 mTargetPosition;
		DirectX::SimpleMath::Quaternion mTargetRotation;
		DirectX::SimpleMath::Vector3 mTargetScale;
	};
}