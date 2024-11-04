#pragma once

#include "SequenceStructure.h"
#include "Track.h"
#include "Camera.h"

namespace fq::game_module
{
	class CameraChangeTrack : public Track
	{
	public:
		CameraChangeTrack();
		~CameraChangeTrack();

		bool Initialize(const CameraChangeTrackInfo& info, Scene* scene);

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

		std::shared_ptr<GameObject> GetCurrentCamera() { return mCurrentCameraObject; }
		std::shared_ptr<GameObject> GetTargetCamera() { return mTargetCameraObject; }
		void SetCurrentCamera(std::shared_ptr<GameObject> camera) { mCurrentCameraObject = camera; }
		void SetTargetCamera(std::shared_ptr<GameObject> camera) { mTargetCameraObject = camera; }

	private:
		std::shared_ptr<GameObject> mCurrentCameraObject;
		std::shared_ptr<GameObject> mTargetCameraObject;

		DirectX::SimpleMath::Matrix mOriginTransform;

		std::vector<TrackKey> mKeys;
	};
}