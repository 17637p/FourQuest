#include "CreateDeleteTrack.h"

#include "Scene.h"
#include "PrefabManager.h"

bool fq::game_module::CreateDeleteTrack::Initialize(const CreateDeleteTrackInfo& info, Scene* scene, const std::vector<PrefabResource>& resource)
{
	mStartTime = info.startTime;
	mTotalPlayTime = info.totalPlayTime;
	mScene = scene;
	mMode = info.mode;

	if (mMode == ECreateDeleteMode::Delete)
	{
		mDeleteName = info.deleteName;
	}
	else
	{
		if (resource.size() > info.prefabIndex)
		{
			mCreateResouce = resource[info.prefabIndex];
		}
		else
		{
			spdlog::trace("[CreateDeleteMode] Index : {} Error", info.prefabIndex);
			return false;
		}
	}

	return true;
}

void fq::game_module::CreateDeleteTrack::End()
{
}

void fq::game_module::CreateDeleteTrack::PlayEnter()
{
	mObject = nullptr;

	switch (mMode)
	{
		case fq::game_module::ECreateDeleteMode::Create:
		{
			auto instance = mScene->GetPrefabManager()->InstantiatePrefabResoure(mCreateResouce);
			mScene->AddGameObject(instance[0]);
		}
		break;
		case fq::game_module::ECreateDeleteMode::Delete:
		{
			auto deleteObject = mScene->GetObjectByName(mDeleteName);

			if (deleteObject)
			{
				mObject = deleteObject;
			}
			else
			{
				spdlog::warn("[CretaeDeleteTrack] {} can't find", mDeleteName);
			}
		}
		break;
		case fq::game_module::ECreateDeleteMode::CreateDelete:
		{
			auto instance = mScene->GetPrefabManager()->InstantiatePrefabResoure(mCreateResouce);
			mScene->AddGameObject(instance[0]);
			mObject = instance[0];
		}
		break;
	}
}

void fq::game_module::CreateDeleteTrack::PlayOn()
{

}

void fq::game_module::CreateDeleteTrack::PlayExit()
{
	// 오브젝트 삭제
	if (mObject)
	{
		if (!mObject->IsDestroyed())
		{
			mScene->DestroyGameObject(mObject.get());
			mObject = nullptr;
		}
		else
		{
			spdlog::warn("[CreateDeleteTrack] {} Object is Destroyed", mObject->GetName());
		}
	}
}

fq::game_module::CreateDeleteTrack::CreateDeleteTrack()
	: Track(ETrackType::CREATE_DELETE)
	, mScene(nullptr)
	, mMode(ECreateDeleteMode::Create)
	, mCreateResouce{}
	, mObject(nullptr)
	, mDeleteName{}
{}

fq::game_module::CreateDeleteTrack::~CreateDeleteTrack()
{

}
