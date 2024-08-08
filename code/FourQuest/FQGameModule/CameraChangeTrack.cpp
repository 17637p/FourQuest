#include "CameraChangeTrack.h"

#include "Scene.h"
#include "GameObject.h"

namespace fq::game_module
{
	CameraChangeTrack::CameraChangeTrack()
		: Track(ETrackType::CAMERA_CHANGE)
	{
	}
	CameraChangeTrack::~CameraChangeTrack()
	{
	}

	bool CameraChangeTrack::Initialize(CameraChangeTrackInfo info, Scene* scene)
	{
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mCurrentCameraObject = scene->GetObjectByName(info.prevCameraName);
		mTargetCameraObject = scene->GetObjectByName(info.targetCameraName);

		if (!mCurrentCameraObject.expired() && !mTargetCameraObject.expired())
			return true;
		else
			return false;
	}

	void CameraChangeTrack::PlayEnter()
	{
		auto camera = mTargetCameraObject.lock()->GetComponent<Camera>();
		camera->SetMainCamera(true);
	}
	void CameraChangeTrack::PlayOn()
	{
	}
	void CameraChangeTrack::PlayExit()
	{
	}
	void CameraChangeTrack::End()
	{
		auto camera = mCurrentCameraObject.lock()->GetComponent<Camera>();
		camera->SetMainCamera(true);
	}
}
