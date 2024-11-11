#include "PlayerMoveTrack.h"

#include "Scene.h"
#include "Transform.h"
#include "CharacterController.h"

namespace fq::game_module
{
	DirectX::SimpleMath::Vector3 QuaternionToYawPitchRoll(const DirectX::SimpleMath::Quaternion& quaternion)
	{
		// 쿼터니언에서 오일러 각 (Yaw, Pitch, Roll)을 추출
		float yaw, pitch, roll;

		// 쿼터니언을 행렬로 변환
		DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(quaternion);

		// Pitch (x축 회전)
		pitch = asinf(-rotationMatrix._32);  // -sin(theta) 값

		// Yaw (y축 회전)
		if (cosf(pitch) > 0.0001f)  // pitch가 +-90도인지 아닌지 확인
		{
			yaw = atan2f(rotationMatrix._31, rotationMatrix._33);  // yaw 값 계산
			roll = atan2f(rotationMatrix._12, rotationMatrix._22); // roll 값 계산
		}
		else
		{
			// pitch가 +-90도일 때 특수한 경우 처리
			yaw = 0.0f;
			roll = atan2f(-rotationMatrix._21, rotationMatrix._11);
		}

		return DirectX::SimpleMath::Vector3(pitch, yaw, roll);  // Pitch, Yaw, Roll 순으로 반환
	}

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
		mbIsPlayerCurrentTransformSetting = info.isPlayerCurrentTransformSetting;
		mPlayerID = info.playerID;
		mKeys = info.keys;

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
				//mTrackObjectName.push_back(object.GetName());
				mTargetObject = mScene->GetObjectByID(object.GetID());
			}
		}

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[PlayerMoveTrack Warrning ({})] Do not Have TargetObject", __LINE__);
			return;
		}

		auto transform = mTargetObject->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();
		
		if (mbIsPlayerCurrentTransformSetting)
		{
			mKeys[0].position = transform->GetWorldPosition();
			mKeys[0].rotation = QuaternionToYawPitchRoll(transform->GetWorldRotation());
			mKeys[0].scale = transform->GetWorldScale();
		}
		transform->SetWorldPosition(mKeys[0].position);
		transform->SetWorldRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[0].rotation));
		transform->SetWorldScale(mKeys[0].scale);
	}

	void PlayerMoveTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

			if (mKeys.size() <= 0)
			{
				spdlog::warn("[ObjectMoverTrack ({})] Warrning Key Size is Zero", __LINE__);
				return;
			}

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
		else
		{
			mTargetObject = nullptr;
		}
	}

	void PlayerMoveTrack::PlayExit()
	{
	}

	void PlayerMoveTrack::End()
	{
		// 시퀀스가 시작하기 이전 Transform로 돌아가야 할 경우
		if (mTargetObject && !mTargetObject->IsDestroyed() && mbIsObjectReturnToStartTransform)
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix prevTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

			transform->SetWorldMatrix(prevTransform);
		}
	}
}
