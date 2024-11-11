#include "PlayerMoveTrack.h"

#include "Scene.h"
#include "Transform.h"
#include "CharacterController.h"

namespace fq::game_module
{
	DirectX::SimpleMath::Vector3 QuaternionToYawPitchRoll(const DirectX::SimpleMath::Quaternion& quaternion)
	{
		// ���ʹϾ𿡼� ���Ϸ� �� (Yaw, Pitch, Roll)�� ����
		float yaw, pitch, roll;

		// ���ʹϾ��� ��ķ� ��ȯ
		DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(quaternion);

		// Pitch (x�� ȸ��)
		pitch = asinf(-rotationMatrix._32);  // -sin(theta) ��

		// Yaw (y�� ȸ��)
		if (cosf(pitch) > 0.0001f)  // pitch�� +-90������ �ƴ��� Ȯ��
		{
			yaw = atan2f(rotationMatrix._31, rotationMatrix._33);  // yaw �� ���
			roll = atan2f(rotationMatrix._12, rotationMatrix._22); // roll �� ���
		}
		else
		{
			// pitch�� +-90���� �� Ư���� ��� ó��
			yaw = 0.0f;
			roll = atan2f(-rotationMatrix._21, rotationMatrix._11);
		}

		return DirectX::SimpleMath::Vector3(pitch, yaw, roll);  // Pitch, Yaw, Roll ������ ��ȯ
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
				//mTrackObjectName.push_back(object.GetName());
				mTargetObject = mScene->GetObjectByID(object.GetID());
			}
		}

		// �ش� ������Ʈ�� �������� ������ �α� ����
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
