#include "FadeTrack.h"

#include "Scene.h"
#include "PostProcessing.h"

namespace fq::game_module
{
	FadeTrack::FadeTrack()
		: Track(ETrackType::FAID_IN_OUT)
		, mScene(nullptr)
		, mPostProcessObject()
		, mBlendTime(1.f)
		, mPrevColor{}
		, mFinalColor{}
		, mPostProcessObjectName{}
	{
	}

	FadeTrack::~FadeTrack()
	{
	}

	bool FadeTrack::Initialize(const FadeTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mBlendTime = info.blendTime;
		mFinalColor = info.finalColor;
		mPostProcessObjectName = info.postProcessObjectName;

		mPostProcessObject = mScene->GetObjectByName(mPostProcessObjectName);

		// 포스트 프로세스 오브젝트가 씬에 존재하지 않으면 실패 반환
		if (mPostProcessObject.expired())
		{
			spdlog::warn("[Warrning] Do not have PostProcessObject");
			return false;
		}

		return true;
	}

	void FadeTrack::PlayEnter()
	{
		// 포스트 프로세스 오브젝트가 씬에 존재하지 않으면 실패 반환
		if (mPostProcessObject.expired())
		{
			spdlog::warn("[Warrning] Do not have PostProcessObject");
		}
		else
		{
			auto postProcessing = mPostProcessObject.lock()->GetComponent<PostProcessing>();
			if (postProcessing == nullptr)
			{
				spdlog::warn("[Warrning] Object have not PostProcessingComponent");
				return;
			}

			auto info = postProcessing->GetPostProcessingInfo();
			mPrevColor = info.BlendColor;
		}
	}

	void FadeTrack::PlayOn()
	{
		if (!mPostProcessObject.expired())
		{
			auto postProcessing = mPostProcessObject.lock()->GetComponent<PostProcessing>();

			float blendTime = 1.f - ((mBlendTime - mElapsedTime) / mBlendTime);
			DirectX::SimpleMath::Color currentColor = DirectX::SimpleMath::Color::Lerp(mPrevColor, mFinalColor, blendTime);

			auto info = postProcessing->GetPostProcessingInfo();
			info.bUseBlendColor = true;
			info.BlendColor = currentColor;

			postProcessing->SetPostProcessingInfo(info);
		}
	}

	void FadeTrack::PlayExit()
	{
	}

	void FadeTrack::End()
	{
	}
}
