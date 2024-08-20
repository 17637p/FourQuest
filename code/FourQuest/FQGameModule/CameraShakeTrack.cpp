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

		// originspacing ������ ���� ��鸰 Ƚ���� ������ �ٽ� �� ��ġ�� �ǵ��ư���. ( ī�޶� �� ��ġ���� �ʹ� ����� �ʱ� ����. )
		if (mDurationTime >= oneShakeTime && mCurrentShakeCount != mOriginInitSpacing)
		{
			mCurrentShakeCount++;
			mDurationTime -= oneShakeTime;

			// ���� ��ġ�� ����� ���� ���ο� ���� ������ ���� ã�� �̵�
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

		// ���� ó���Ͽ� ī�޶��� ��ġ�� �������ش�.
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
