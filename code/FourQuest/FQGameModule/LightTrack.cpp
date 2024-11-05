#include "LightTrack.h"

#include "Scene.h"
#include "Light.h"

namespace fq::game_module
{
	LightTrack::LightTrack()
		: Track(ETrackType::LIGHT)
		, mTargetObject()
		, mScene()
		, mLightTrackKeys{}
		, mTargetObjectName()
		, mPrevIntensity()
	{
	}

	LightTrack::~LightTrack()
	{
	}

	bool LightTrack::Initialize(const LightTrackInfo& info, Scene* scene)
	{
		mTrackObjectName.push_back(info.lightObjectName);

		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mTargetObjectName = info.lightObjectName;
		mLightTrackKeys = info.lightTrackKeys;
		mScene = scene;

		return true;
	}

	void LightTrack::PlayEnter()
	{
		mTargetObject = mScene->GetObjectByName(mTargetObjectName);

		// �ش� ������Ʈ�� �������� ������ �α� ����
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[LightTrack Warrning ({})] Do not Have TargetObject {}", __LINE__, mTargetObjectName);
			return;
		}

		if (!mTargetObject->HasComponent<Light>())
		{
			spdlog::warn("[LightTrack Warrning ({})] TargetObject Have not Light Component", __LINE__);
			return;
		}

		// time ���� ���� TrackKey ���͸� ������������ ����
		std::sort(mLightTrackKeys.begin(), mLightTrackKeys.end(), [](const LightTrackKey& a, const LightTrackKey& b)
			{
				return a.time < b.time;
			});

		auto lightComponent = mTargetObject->GetComponent<Light>();
		mPrevIntensity = lightComponent->GetIntensity();
	}

	void LightTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			auto LightComponent = mTargetObject->GetComponent<Light>();

			// ���� ������� Ű�� �������� ã��
			for (int i = 0; i < mLightTrackKeys.size(); i++)
			{
				if (mElapsedTime >= mLightTrackKeys[i].time && checkPointTime < mLightTrackKeys[i].time)
				{
					checkPointTime = mLightTrackKeys[i].time;
					keyNumber = i;
				}
			}

			// ���� Ű�� ���� Ű�� Ʈ�������� ���Ͽ� Lerp ������ �����̱�
			// �̹� �ִ� Ű���̶�� �ش� Ű���� Transform���� �����ϱ�
			if (keyNumber + 1 < mLightTrackKeys.size())
			{
				float divisionValue = mLightTrackKeys[keyNumber + 1].time - mLightTrackKeys[keyNumber].time;

				if (divisionValue == 0.f)
				{
					spdlog::warn("[LightTrack] divisionValue == 0.f ");
					divisionValue = 1.f;
				}

				// ������
				float lerpValue = (mElapsedTime - mLightTrackKeys[keyNumber].time) / divisionValue;

				// ������ ��� ��
				float currentIntensity = std::lerp(mLightTrackKeys[keyNumber].intensity, mLightTrackKeys[keyNumber + 1].intensity, lerpValue);

				// Light ������Ʈ�� ��� �����ϱ�
				LightComponent->SetIntensity(currentIntensity);
			}
			else
			{
				// ������ ��
				float currentIntensity = mLightTrackKeys[mLightTrackKeys.size() - 1].intensity;

				// Light ������Ʈ�� ��� �����ϱ�
				LightComponent->SetIntensity(currentIntensity);
			}
		}
		else
		{
			mTargetObject = nullptr;
		}
	}

	void LightTrack::PlayExit()
	{
	}

	void LightTrack::End()
	{
		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			auto LightComponent = mTargetObject->GetComponent<Light>();

			LightComponent->SetIntensity(mPrevIntensity);
		}
	}
}
