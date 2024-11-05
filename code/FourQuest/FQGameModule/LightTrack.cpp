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

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
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

		// time 값에 따라 TrackKey 벡터를 오름차순으로 정렬
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

			// 현재 재생중인 키가 무엇인지 찾기
			for (int i = 0; i < mLightTrackKeys.size(); i++)
			{
				if (mElapsedTime >= mLightTrackKeys[i].time && checkPointTime < mLightTrackKeys[i].time)
				{
					checkPointTime = mLightTrackKeys[i].time;
					keyNumber = i;
				}
			}

			// 현재 키와 다음 키의 트랜스폼을 비교하여 Lerp 값으로 움직이기
			// 이미 최대 키값이라면 해당 키값의 Transform으로 세팅하기
			if (keyNumber + 1 < mLightTrackKeys.size())
			{
				float divisionValue = mLightTrackKeys[keyNumber + 1].time - mLightTrackKeys[keyNumber].time;

				if (divisionValue == 0.f)
				{
					spdlog::warn("[LightTrack] divisionValue == 0.f ");
					divisionValue = 1.f;
				}

				// 보간값
				float lerpValue = (mElapsedTime - mLightTrackKeys[keyNumber].time) / divisionValue;

				// 보간한 밝기 값
				float currentIntensity = std::lerp(mLightTrackKeys[keyNumber].intensity, mLightTrackKeys[keyNumber + 1].intensity, lerpValue);

				// Light 컴포넌트에 밝기 세팅하기
				LightComponent->SetIntensity(currentIntensity);
			}
			else
			{
				// 마지막 값
				float currentIntensity = mLightTrackKeys[mLightTrackKeys.size() - 1].intensity;

				// Light 컴포넌트에 밝기 세팅하기
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
