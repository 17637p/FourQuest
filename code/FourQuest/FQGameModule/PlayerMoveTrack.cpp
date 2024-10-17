#include "PlayerMoveTrack.h"

#include "Scene.h"
#include "Transform.h"
#include "CharacterController.h"

namespace fq::game_module
{
	PlayerMoveTrack::PlayerMoveTrack()
		: Track(ETrackType::PLAYER_MOVE)
		, mTargetObject()
		, mKeys{}
		, mPlayerID()
		, mbIsObjectReturnToStartTransform{}
		, mPrevPosition{}
		, mPrevRotation{}
		, mPrevScale{}
	{
	}

	PlayerMoveTrack::~PlayerMoveTrack()
	{
	}

	bool PlayerMoveTrack::Initialize(const PlayerMoveTrackInfo& info, Scene* scene)
	{
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;
		mPlayerID = info.playerID;

		// 컨트롤러를 탐색해서 플레이어 찾기
		for (auto& object : scene->GetComponentView<CharacterController>())
		{
			auto characterController = object.GetComponent<CharacterController>();

			if (info.playerID == characterController->GetControllerID())
			{
				mTrackObjectName.push_back(object.GetName());
				mTargetObject = scene->GetObjectByName(object.GetName());
			}
		}

		if (mTargetObject.expired())
			return false;

		if (!mTargetObject.lock()->HasComponent<Transform>()) return false;

		mKeys = info.keys;

		auto transform = mTargetObject.lock()->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		return true;
	}

	void PlayerMoveTrack::PlayEnter()
	{
		// time 값에 따라 TrackKey 벡터를 오름차순으로 정렬
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b)
			{
				return a.time < b.time;
			});
	}

	void PlayerMoveTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

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

	void PlayerMoveTrack::PlayExit()
	{
	}

	void PlayerMoveTrack::End()
	{
		if (!mTargetObject.expired() && mbIsObjectReturnToStartTransform)
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix prevTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

			transform->SetWorldMatrix(prevTransform);
		}
	}
}
