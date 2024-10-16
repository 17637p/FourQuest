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
		mScene = scene;
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;
		mPlayerID = info.playerID;

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

		// 컨트롤러를 탐색해서 플레이어 찾기
		for (auto& object : mScene->GetComponentView<CharacterController>())
		{
			auto characterController = object.GetComponent<CharacterController>();

			if (mPlayerID == characterController->GetControllerID())
			{
				mTrackObjectName.push_back(object.GetName());
				mTargetObject = mScene->GetObjectByName(object.GetName());
			}
		}

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject.expired())
		{
			spdlog::warn("[Warrning] Do not Have TargetObject");
		}
	}

	void PlayerMoveTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			// 현재 재생중인 키가 무엇인지 찾기
			for (int i = 0; i < mKeys.size(); i++)
			{
				if (mElapsedTime >= mKeys[i].time && checkPointTime < mKeys[i].time)
				{
					checkPointTime = mKeys[i].time;
					keyNumber = i;
				}
			}

			// 현재 키와 다음 키의 트랜스폼을 비교하여 Lerp 값으로 움직이기
			// 이미 최대 키값이라면 해당 키값의 Transform으로 세팅하기
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
		// 시퀀스가 시작하기 이전 Transform로 돌아가야 할 경우
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
