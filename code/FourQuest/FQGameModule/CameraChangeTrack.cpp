#include "CameraChangeTrack.h"

#include "Scene.h"
#include "GameObject.h"

#include "Transform.h"



namespace fq::game_module
{
	CameraChangeTrack::CameraChangeTrack()
		: Track(ETrackType::CAMERA_CHANGE)
	{
	}
	CameraChangeTrack::~CameraChangeTrack()
	{
	}

	bool CameraChangeTrack::Initialize(const CameraChangeTrackInfo& info, Scene* scene)
	{
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mKeys = info.keys;

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
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (!mTargetCameraObject.expired())
		{
			if (!mTargetCameraObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetCameraObject.lock()->GetComponent<Transform>();

			for (int i = 0; i < mKeys.size(); i++)
			{
				if (mElapsedTime >= mKeys[i].time && checkPointTime < mKeys[i].time)
				{
					checkPointTime = mKeys[i].time;
					keyNumber = i;
				}
			}

			if (keyNumber + 1 < mKeys.size())
			{
				float divisionValue = mKeys[keyNumber + 1].time - mKeys[keyNumber].time;
				float lerpValue = (mElapsedTime - mKeys[keyNumber].time) / divisionValue;

				DirectX::SimpleMath::Vector3 scale = LerpVector3(mKeys[keyNumber].scale, mKeys[keyNumber + 1].scale, lerpValue);
				DirectX::SimpleMath::Vector3 rotation = LerpVector3(mKeys[keyNumber].rotation, mKeys[keyNumber + 1].rotation, lerpValue);
				DirectX::SimpleMath::Vector3 position = LerpVector3(mKeys[keyNumber].position, mKeys[keyNumber + 1].position, lerpValue);

				DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation / 180.f * 3.14f);

				DirectX::SimpleMath::Matrix newTransform =
					DirectX::SimpleMath::Matrix::CreateScale(scale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(quaternion)
					* DirectX::SimpleMath::Matrix::CreateTranslation(position);

				transform->SetWorldMatrix(newTransform);
			}
			else
			{
				DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[mKeys.size() - 1].rotation / 180.f * 3.14f);

				DirectX::SimpleMath::Matrix newTransform =
					DirectX::SimpleMath::Matrix::CreateScale(mKeys[mKeys.size() - 1].scale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(quaternion)
					* DirectX::SimpleMath::Matrix::CreateTranslation(mKeys[mKeys.size() - 1].position);

				transform->SetWorldMatrix(newTransform);
			}
		}
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
