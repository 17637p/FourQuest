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
#include "TimeManager.h"
#include "TextUI.h"

namespace fq::game_module
{
	Sequence::Sequence()
		: mTracks()
		, mbIsPlay(false)
		, mbIsLoop(false)
		, mbIsOnce(true)
		, mbIsTimeStop(false)
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
			float deltaTime = GetScene()->GetTimeManager()->GetDeltaTime();
			mDurationTime += deltaTime;

			playTrack(deltaTime);
			updateUI();
			updateSequenceObject(deltaTime);

			if (mDurationTime >= mTotalPlayTime)
			{
				mDurationTime = 0;

				if (!mbIsLoop)
				{
					mbIsPlay = false;
				}

				// �� ���� ����Ǵ� �Ŷ�� �� �� ����� ���� �� ������ ����
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

	void Sequence::OnDestroy()
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });
	}

	void Sequence::OnTriggerEnter(const Collision& collision)
	{
		if (collision.object->HasComponent<fq::client::Player>())
		{
			mbIsPlay = true;

			for (const auto& track : mTracks)
				track->WakeUp();
		}
	}

	void Sequence::playTrack(float dt)
	{
		for (const auto& track : mTracks)
		{
			if (dt == 0.f && track->GetType() == ETrackType::TEXT_PRINT)
				continue;

			track->Play(mDurationTime);
		}

		if (mDurationTime >= mTotalPlayTime)
		{
			for (const auto& track : mTracks)
			{
				track->WakeUp();
				track->End();
			}
		}
	}

	void Sequence::updateUI()
	{
		// UI ����
		if (mbIsOffUIRender)
			GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });

		// UI �ѱ�
		if (mDurationTime >= mTotalPlayTime)
			GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });
	}

	void Sequence::updateSequenceObject(float dt)
	{
		if (!mbIsTimeStop)
			return;

		GetScene()->GetTimeManager()->SetTimeScale(0.01f);

		for (const auto& track : mTracks)
		{
			for (auto& objectName : track->GetTrackObjectName())
			{
				auto object = GetScene()->GetObjectByName(objectName);

				if (!object)
					continue;

				if (!object->HasComponent<TextUI>())
					object->OnUpdate(dt);

				for (auto child : object->GetChildren())
				{
					if (!child)
						continue;

					if (!child->HasComponent<TextUI>())
						object->OnUpdate(dt);
				}
			}
		}

		if (mDurationTime >= mTotalPlayTime)
			GetScene()->GetTimeManager()->SetTimeScale(1.f);
	}

	entt::meta_handle Sequence::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<Component> Sequence::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneSequence = std::dynamic_pointer_cast<Sequence>(clone);

		if (cloneSequence == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneSequence = ObjectPool::GetInstance()->Assign<Sequence>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
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
