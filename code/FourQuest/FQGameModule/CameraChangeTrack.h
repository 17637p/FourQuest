#pragma once

#include <string>

#include "Track.h"
#include "Camera.h"

namespace fq::game_module
{
	class CameraChangeTrack : public Track
	{
	public:
		CameraChangeTrack(ETrackType type);
		~CameraChangeTrack();

		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;

		std::weak_ptr<Camera> GetCurrentCamera() { return mCurrentCamera; }
		std::weak_ptr<Camera> GetTargetCamera() { return mTargetCamera; }
		void SetCurrentCamera(std::weak_ptr<Camera> camera) { mCurrentCamera = camera; }
		void SetTargetCamera(std::weak_ptr<Camera> camera) { mTargetCamera = camera; }

	private:
		std::weak_ptr<Camera> mCurrentCamera;
		std::weak_ptr<Camera> mTargetCamera;
	};
}