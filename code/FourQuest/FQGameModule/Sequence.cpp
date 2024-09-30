#include "Sequence.h"

#include "..\FQClient\Player.h"

#include "Scene.h"
#include "Track.h"
#include "CameraChangeTrack.h"
#include "CameraShakeTrack.h"
#include "ObjectMoveTrack.h"
#include "ObjectTeleportTrack.h"
#include "TextPrintTrack.h"
#include "EffectTrack.h"
#include "SoundTrack.h"
#include "ObjectAnimationTrack.h"
#include "EventManager.h"
#include "Event.h"

namespace fq::game_module
{
	Sequence::Sequence()
		: mTracks()
		, mbIsPlay(false)
		, mbIsLoop(false)
		, mbIsOnce(true)
		, mbIsOffUIRender(false)
		, mTotalPlayTime(0.f)
		, mDurationTime(0.f)
		, mAnimationContainer{}
		, mCameraChangeTrackInfo{}
		, mObjectMoveTrackInfo{}
		, mObjectTeleportTrackInfo{}
		, mObjectAnimationInfo{}
		, mEffectTrackInfo{}
		, mSoundTrackInfo{}
		, mTextPrintTrackInfo{}
	{
	}

	Sequence::~Sequence()
	{
	}

	void Sequence::OnStart()
	{
		auto scene = GetScene();
		bool check = false;

		for (const auto& trackInfo : mCameraChangeTrackInfo)
		{
			std::shared_ptr<CameraChangeTrack> track = std::make_shared<CameraChangeTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mObjectMoveTrackInfo)
		{
			std::shared_ptr<ObjectMoveTrack> track = std::make_shared<ObjectMoveTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mObjectTeleportTrackInfo)
		{
			std::shared_ptr<ObjectTeleportTrack> track = std::make_shared<ObjectTeleportTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mObjectAnimationInfo)
		{
			std::shared_ptr<ObjectAnimationTrack> track = std::make_shared<ObjectAnimationTrack>();
			check = track->Initialize(trackInfo, scene, mAnimationContainer.find(trackInfo.targetObjectName)->second);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mSoundTrackInfo)
		{
			std::shared_ptr<SoundTrack> track = std::make_shared<SoundTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mEffectTrackInfo)
		{
			std::shared_ptr<EffectTrack> track = std::make_shared<EffectTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mTextPrintTrackInfo)
		{
			std::shared_ptr<TextPrintTrack> track = std::make_shared<TextPrintTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
		for (const auto& trackInfo : mCameraShakeTrackInfo)
		{
			std::shared_ptr<CameraShakeTrack> track = std::make_shared<CameraShakeTrack>();
			check = track->Initialize(trackInfo, scene);

			if (check)
			{
				mTracks.push_back(track);

				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
		}
	}

	void Sequence::OnUpdate(float dt)
	{
		if (mbIsPlay)
		{
			mDurationTime += dt;

			// UI 끄기
			if (mbIsOffUIRender)
				GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });

			for (const auto& track : mTracks)
			{
				if (dt == 0.f && track->GetType() == ETrackType::TEXT_PRINT)
					continue;

				track->Play(mDurationTime);
			}


			if (mDurationTime >= mTotalPlayTime)
			{
				mDurationTime = 0;

				// UI 켜기
				GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });

				for (const auto& track : mTracks)
				{
					track->WakeUp();
					track->End();
				}

				if (!mbIsLoop)
				{
					mbIsPlay = false;
				}

				// 한 번만 재생되는 거라면 한 번 재생이 끝난 후 시퀀스 제거
				if (mbIsOnce)
				{
					GetScene()->DestroyGameObject(GetGameObject());
				}
			}
		}
		else
		{
			mDurationTime = 0.f;
		}
	}

	void Sequence::OnTriggerEnter(const Collision& collision)
	{
		//if (collision.object->HasComponent<fq::client::Player>())
		{
			mbIsPlay = true;

			for (const auto& track : mTracks)
				track->WakeUp();
		}
	}

	entt::meta_handle Sequence::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<Component> Sequence::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneSequence = std::dynamic_pointer_cast<Sequence>(clone);

		if (cloneSequence == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneSequence = ObjectPool::GetInstance()->Assign<Sequence>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneSequence = *this;
		}

		return cloneSequence;
	}

	void Sequence::SetDurationTime(float durationTime)
	{
		mDurationTime = durationTime;

		for (const auto track : mTracks)
		{
			track->WakeUp();
			track->End();

			mbIsPlay = true;
			OnFixedUpdate(0.0f);
			OnFixedUpdate(0.0f);
			mbIsPlay = false;
		}
	}
}
