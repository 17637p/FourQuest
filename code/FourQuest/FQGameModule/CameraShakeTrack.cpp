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
		, mOrizinPosition{}
		, mPrevPoisiton{}
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

		auto transform = mCameraObject->GetComponent<Transform>();

		if (transform != nullptr)
		{
			mOrizinPosition = transform->GetLocalPosition();
			mPrevPoisiton = transform->GetLocalPosition();
		}

		mTargetPosition.x = mOrizinPosition.x + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.x;
		mTargetPosition.y = mOrizinPosition.y + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.y;
		mTargetPosition.z = mOrizinPosition.z + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.z;
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

		mDurationTime += deltaTime;

		// originspacing 변수와 현재 흔들린 횟수가 같으면 다시 원 위치로 되돌아간다. ( 카메라가 원 위치에서 너무 벗어나지 않기 위함. )
		if (mDurationTime >= oneShakeTime && mCurrentShakeCount != mOriginInitSpacing)
		{
			mCurrentShakeCount++;
			mDurationTime -= oneShakeTime;

			// 이전 위치를 기억한 이후 새로운 진동 오프셋 값을 찾아 이동
			mPrevPoisiton = mTargetPosition;
			mTargetPosition.x = mPrevPoisiton.x + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.x;
			mTargetPosition.y = mPrevPoisiton.y + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.y;
			mTargetPosition.z = mPrevPoisiton.z + ((rand() % 1000) / 1000.f - 0.5f) * 2 * mMagnitude.z;
		}
		else if (mDurationTime >= oneShakeTime && mCurrentShakeCount == mOriginInitSpacing)
		{
			mCurrentShakeCount++;
			mDurationTime -= oneShakeTime;

			mPrevPoisiton = mTargetPosition;
			mTargetPosition.x = mOrizinPosition.x;
			mTargetPosition.y = mOrizinPosition.y;
			mTargetPosition.z = mOrizinPosition.z;
		}

		// 보간 처리하여 카메라의 위치를 조정해준다.
		mCurrentPoisiton.x = std::lerp(mPrevPoisiton.x, mTargetPosition.x, normalizedTime / oneShakeTime);
		mCurrentPoisiton.y = std::lerp(mPrevPoisiton.y, mTargetPosition.y, normalizedTime / oneShakeTime);
		mCurrentPoisiton.z = std::lerp(mPrevPoisiton.z, mTargetPosition.z, normalizedTime / oneShakeTime);

		auto transform = mCameraObject->GetComponent<Transform>();

		if (transform != nullptr)
			transform->SetLocalPosition(mCurrentPoisiton);
	}

	void CameraShakeTrack::PlayExit()
	{
		if (mCameraObject != nullptr)
		{
			auto transform = mCameraObject->GetComponent<Transform>();

			if (transform != nullptr)
			{
				transform->SetLocalPosition(mCurrentPoisiton);
			}
		}

		mCurrentShakeCount = 0;
		mDurationTime = 0.f;
		mTargetPosition = {};
	}

	void CameraShakeTrack::End()
	{
	}

}
