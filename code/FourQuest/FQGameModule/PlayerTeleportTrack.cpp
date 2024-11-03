#define NOMINMAX 
#include "PlayerTeleportTrack.h"

#include "Scene.h"
#include "CharacterController.h"
#include "Transform.h"

namespace fq::game_module
{
	PlayerTeleportTrack::PlayerTeleportTrack()
		: Track(ETrackType::PLAYER_TELEPORT)
		, mTargetObject()
		, mKeys{}
		, mPlayerID()
		, mbIsObjectReturnToStartTransform{}
		, mPrevPosition{}
		, mPrevRotation{}
		, mPrevScale{}
	{
	}

	PlayerTeleportTrack::~PlayerTeleportTrack()
	{
	}

	bool PlayerTeleportTrack::Initialize(const PlayerTeleportTrackInfo& info, Scene* scene)
	{
		mScene = scene;
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;
		mPlayerID = info.playerID;
		mKeys = info.keys;

		return true;
	}

	void PlayerTeleportTrack::PlayEnter()
	{
		// time ���� ���� TrackKey ���͸� ������������ ����
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b)
			{
				return a.time < b.time;
			});

		// ��Ʈ�ѷ��� Ž���ؼ� �÷��̾� ã��
		for (auto& object : mScene->GetComponentView<CharacterController>())
		{
			auto characterController = object.GetComponent<CharacterController>();

			if (mPlayerID == characterController->GetControllerID())
			{
				mTrackObjectName.push_back(object.GetName());
				mTargetObject = mScene->GetObjectByID(object.GetID());
			}
		}

		// �ش� ������Ʈ�� �������� ������ �α� ����
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[PlayerTeleportTrack Warrning({})] Do not Have TargetObject", __LINE__);
			return;
		}

		auto transform = mTargetObject->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		transform->SetWorldPosition(mKeys[0].position);
		transform->SetWorldRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[0].rotation));
		transform->SetWorldScale(mKeys[0].scale);
	}

	void PlayerTeleportTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

			for (int i = 0; i < mKeys.size(); i++)
			{
				if (mElapsedTime >= mKeys[i].time && checkPointTime < mKeys[i].time)
				{
					checkPointTime = mKeys[i].time;
					keyNumber = i;
				}
			}

			DirectX::SimpleMath::Matrix teleportTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mKeys[keyNumber].scale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[keyNumber].rotation / 180.f * 3.14f))
				* DirectX::SimpleMath::Matrix::CreateTranslation(mKeys[keyNumber].position);

			transform->SetWorldMatrix(teleportTransform);
		}
		else
		{
			mTargetObject = nullptr;
		}
	}

	void PlayerTeleportTrack::PlayExit()
	{
	}

	void PlayerTeleportTrack::End()
	{
		// �������� �����ϱ� ���� Transform�� ���ư��� �� ���
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
