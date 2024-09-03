#include "CameraShakeTrack.h"

#include "Scene.h"
#include "Transform.h"

namespace fq::game_module
{
	CameraShakeTrack::CameraShakeTrack()
		: Track(ETrackType::CAMERA_SHAKE)
		, mMagnitude{}
		, mShakeCount()
		, mCameraObjectName{}
		, mOriginInitSpacing()
		, mTargetPosition{}
		, mPrevPosition{}
		, mOffsetNextPosition{}
		, mOffsetPrevPosition{}
		, mCurrentTime()
		, mPrevTime()
		, mCurrentShakeCount()
		, mDurationTime()
	{
	}

	CameraShakeTrack::~CameraShakeTrack()
	{
	}

	bool CameraShakeTrack::Initialize(const CameraShakeTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		mTotalPlayTime = info.totalPlayTime;
		mStartTime = info.startTime;

		mShakeCount = info.shakeCount;
		mOriginInitSpacing = info.originInitSpacing;
		mMagnitude = info.magnitude;
		mCameraObjectName = info.cameraObjectName;

		return true;
	}

	void CameraShakeTrack::PlayEnter()
	{
		mCameraObject = mScene->GetObjectByName(mCameraObjectName).get();

		GenerateNewShakeOffset();
		mDurationTime = 0.f;
	}

	void CameraShakeTrack::PlayOn()
	{
		if (mCameraObject == nullptr)
			return;

		mCurrentTime = mElapsedTime;
		float deltaTime = mCurrentTime - mPrevTime;
		mPrevTime = mElapsedTime;

		float oneShakeTime = mTotalPlayTime / mShakeCount;
		float normalizedTime = std::fmod(mElapsedTime, oneShakeTime);

		if (deltaTime >= 0.f)
			mDurationTime += deltaTime;

		if (mDurationTime >= oneShakeTime)
		{
			mCurrentShakeCount++;
			mDurationTime -= oneShakeTime;

			mPrevPosition = mTargetPosition;

			if (mCurrentShakeCount > mShakeCount || mCurrentShakeCount == mOriginInitSpacing)
			{
				mTargetPosition = -mCameraOffsetPosition;
				mCameraOffsetPosition = {};
			}
			else if (mCurrentShakeCount < mShakeCount)
			{
				GenerateNewShakeOffset();
			}
		}

		// 보간 처리하여 카메라의 위치를 조정해준다.
		mOffsetNextPosition.x = std::lerp(mPrevPosition.x, mTargetPosition.x, normalizedTime / oneShakeTime);
		mOffsetNextPosition.y = std::lerp(mPrevPosition.y, mTargetPosition.y, normalizedTime / oneShakeTime);
		mOffsetNextPosition.z = std::lerp(mPrevPosition.z, mTargetPosition.z, normalizedTime / oneShakeTime);
		DirectX::SimpleMath::Vector3 addPosition = mOffsetNextPosition - mOffsetPrevPosition;
		mOffsetPrevPosition = mOffsetNextPosition;

		mCameraOffsetPosition += addPosition;

		auto transform = mCameraObject->GetComponent<Transform>();

		if (transform != nullptr)
			transform->AddLocalPosition(addPosition);
	}

	void CameraShakeTrack::PlayExit()
	{
		//if (mCameraObject != nullptr)
		//{
		//	auto transform = mCameraObject->GetComponent<Transform>();
		//	if (transform != nullptr)
		//	{
		//		// 누적된 오프셋만큼 원래 위치로 복구
		//		transform->AddLocalPosition(-mCameraOffsetPosition);
		//	}
		//}

		mCurrentShakeCount = 0;
		mDurationTime = 0.f;
		mTargetPosition = {};
		mPrevPosition = {};
		mOffsetNextPosition = {};
		mOffsetPrevPosition = {};
		mCameraOffsetPosition = {}; // 초기화
	}

	void CameraShakeTrack::End()
	{
	}

	void CameraShakeTrack::GenerateNewShakeOffset()
	{
		// 새로운 진동 오프셋 값 생성
		mTargetPosition.x = ((rand() % 1000) / 1000.f - 0.5f) * 2;
		mTargetPosition.y = ((rand() % 1000) / 1000.f - 0.5f) * 2;
		mTargetPosition.z = ((rand() % 1000) / 1000.f - 0.5f) * 2;

		mTargetPosition.Normalize();

		mTargetPosition.x *= mMagnitude.x;
		mTargetPosition.y *= mMagnitude.y;
		mTargetPosition.z *= mMagnitude.z;
	}
}
