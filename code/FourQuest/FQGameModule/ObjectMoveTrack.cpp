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
		mTrackObjectName.push_back(info.targetObjectName);
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;

		mTargetObject = scene->GetObjectByName(info.targetObjectName);

		// �ش� ������Ʈ�� �������� ������ �α� ����
		if (mTargetObject.expired())
		{
			spdlog::warn("[ObjectMoveTrack Warrning({})] Do not Have TargetObject", __LINE__);
			return false;
		}

		// �ش� ������Ʈ�� Transform�� ������ ���� ������ �α� ����
		if (!mTargetObject.lock()->HasComponent<Transform>())
		{
			spdlog::warn("[ObjectMoveTrack Warrning({})] TargetObject Have not Trasfrom Component", __LINE__);
			return false;
		}

		mKeys = info.keys;

		auto transform = mTargetObject.lock()->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		return true;
	}

	void ObjectMoveTrack::PlayEnter()
	{
		// time ���� ���� TrackKey ���͸� ������������ ����
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b) 
			{
			return a.time < b.time;
			});
	}

	void ObjectMoveTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			// ���� ������� Ű�� �������� ã��
			for (int i = 0; i < mKeys.size(); i++)
			{
				if (mElapsedTime >= mKeys[i].time && checkPointTime < mKeys[i].time)
				{
					checkPointTime = mKeys[i].time;
					keyNumber = i;
				}
			}

			// ���� Ű�� ���� Ű�� Ʈ�������� ���Ͽ� Lerp ������ �����̱�
			// �̹� �ִ� Ű���̶�� �ش� Ű���� Transform���� �����ϱ�
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

	void ObjectMoveTrack::PlayExit()
	{
	}

	void ObjectMoveTrack::End()
	{
		// �������� �����ϱ� ���� Transform�� ���ư��� �� ���
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
