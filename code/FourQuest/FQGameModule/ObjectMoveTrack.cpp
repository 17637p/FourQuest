#include "ObjectMoveTrack.h"

#include "Scene.h"
#include "Transform.h"


namespace fq::game_module
{
	ObjectMoveTrack::ObjectMoveTrack()
		: Track(ETrackType::OBJECT_MOVE)
		, mbIsObjectReturnToStartTransform()
		, mTargetObject()
		, mKeys{}
		, mPrevPosition{}
		, mPrevRotation{}
		, mPrevScale{}
	{
	}
	ObjectMoveTrack::~ObjectMoveTrack()
	{
	}

	bool ObjectMoveTrack::Initialize(const ObjectMoveTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		//mTrackObjectName.push_back(info.targetObjectName);
		mTargetObjectName = info.targetObjectName;
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;
		mKeys = info.keys;

		return true;
	}

	void ObjectMoveTrack::PlayEnter()
	{
		mTargetObject = mScene->GetObjectByName(mTargetObjectName);

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[ObjectMoveTrack Warrning({})] Do not Have TargetObject : {}", __LINE__, mTargetObjectName);
			return;
		}
		// 해당 오브젝트가 Transform을 가지고 있지 않으면 로그 띄우기
		else if (!mTargetObject->HasComponent<Transform>())
		{
			spdlog::warn("[ObjectMoveTrack Warrning({})] TargetObject Have not Trasfrom Component", __LINE__);
		}

		auto transform = mTargetObject->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		// time 값에 따라 TrackKey 벡터를 오름차순으로 정렬
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b)
			{
				return a.time < b.time;
			});
	}

	void ObjectMoveTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

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

				if (divisionValue == 0.f)
				{
					spdlog::warn("[ObjectTrack] divisionValue == 0.f ");
					divisionValue = 1.f;
				}

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

	void ObjectMoveTrack::PlayExit()
	{
		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			auto transform = mTargetObject->GetComponent<Transform>();
			if (mbIsObjectReturnToStartTransform)
			{
				DirectX::SimpleMath::Matrix prevTransform =
					DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
					* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

				transform->SetWorldMatrix(prevTransform);
			}
			else
			{
				const auto& lastKey = mKeys.back();
				auto rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(lastKey.rotation / 180.f * 3.14f);
				transform->GenerateWorld(lastKey.position, rotation, lastKey.scale);
			}
		}
		else
		{
			mTargetObject = nullptr;
		}
	}

	void ObjectMoveTrack::End()
	{
		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			auto transform = mTargetObject->GetComponent<Transform>();
			if (mbIsObjectReturnToStartTransform)
			{
				DirectX::SimpleMath::Matrix prevTransform =
					DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
					* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

				transform->SetWorldMatrix(prevTransform);
			}
			else
			{
				const auto& lastKey = mKeys.back();
				auto rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(lastKey.rotation / 180.f * 3.14f);
				transform->GenerateWorld(lastKey.position, rotation, lastKey.scale);
			}
		}
		else
		{
			mTargetObject = nullptr;
		}
	}
}
