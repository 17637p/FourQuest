#include "EffectTrack.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

#include "PrefabManager.h"

namespace fq::game_module
{
	EffectTrack::EffectTrack()
		: Track(ETrackType::EFFECT)
		, mScene(nullptr)
		, mPrefebPath{}
		, mKeys{}
		, mEffectObject{}
	{
	}
	EffectTrack::~EffectTrack()
	{
	}

	bool EffectTrack::Initialize(const EffectTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mPrefebPath = info.effectPath;
		mKeys = info.keys;

		return true;
	}

	void EffectTrack::PlayEnter()
	{
		mObjects = mScene->GetPrefabManager()->LoadPrefab(mPrefebPath);
		
		for (auto object : mObjects)
		{
			mTrackObjectName.push_back(object->GetName());
		}

		assert(!mObjects.empty());

		for (const auto& object : mObjects)
		{
			if (object->GetParent() == nullptr)
			{
				mEffectObject = object;
				mScene->AddGameObject(object);
			}
		}

		auto transform = mEffectObject.lock()->GetComponent<Transform>();
		transform->SetWorldPosition(mKeys[0].position);
		transform->SetWorldRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[0].rotation));
		transform->SetWorldScale(mKeys[0].scale);
	}

	void EffectTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (!mEffectObject.expired())
		{
			if (!mEffectObject.lock()->HasComponent<Transform>()) return;

			auto transform = mEffectObject.lock()->GetComponent<Transform>();

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

				transform->SetWorldPosition(position);
				transform->SetWorldRotation(quaternion);
				transform->SetWorldScale(scale);
			}
		}
	}

	void EffectTrack::PlayExit()
	{
		if (!mEffectObject.expired())
		{
			mScene->DestroyGameObject(mEffectObject.lock().get());
			mObjects.clear();
		}
	}

	void EffectTrack::End()
	{
		if (!mEffectObject.expired())
		{
			mScene->DestroyGameObject(mEffectObject.lock().get());
			mObjects.clear();
		}
	}
}
