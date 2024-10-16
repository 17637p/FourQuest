#include "Sequence.h"

#include "..\FQClient\Player.h"
#include "CharacterController.h"


#include "EventManager.h"
#include "Event.h"
#include "TimeManager.h"
#include "InputManager.h"
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
		, mCameraShakeTrackInfo{}
		, mObjectMoveTrackInfo{}
		, mObjectTeleportTrackInfo{}
		, mPlayerMoveTrackInfo{}
		, mPlayerTeleportTrackInfo{}
		, mObjectAnimationInfo{}
		, mEffectTrackInfo{}
		, mSoundTrackInfo{}
		, mTextPrintTrackInfo{}
		, mVibrationTrackInfo{}
	{
	}

	Sequence::~Sequence()
	{

	}

	void Sequence::OnStart()
	{
		createTrack<CameraChangeTrack>(mCameraChangeTrackInfo);
		createTrack<CameraShakeTrack>(mCameraShakeTrackInfo);
		createTrack<ObjectMoveTrack>(mObjectMoveTrackInfo);
		createTrack<ObjectTeleportTrack>(mObjectTeleportTrackInfo);
		createTrack<PlayerMoveTrack>(mPlayerMoveTrackInfo);
		createTrack<PlayerTeleportTrack>(mPlayerTeleportTrackInfo);
		createTrack<EffectTrack>(mEffectTrackInfo);
		createTrack<SoundTrack>(mSoundTrackInfo);
		createTrack<TextPrintTrack>(mTextPrintTrackInfo);
		createTrack<VibrationTrack>(mVibrationTrackInfo);

		auto scene = GetScene();
		bool check = false;

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
	}

	void Sequence::OnUpdate(float dt)
	{
		if (mbIsPlay)
		{
			checkSeqeunce();

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

	void Sequence::OnDestroy()
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });

		auto input = GetScene()->GetInputManager();

		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			input->SetVibration(i, EVibrationMode::Both, 0.f, mTotalPlayTime);
		}
	}

	void Sequence::OnTriggerEnter(const Collision& collision)
	{
		if (collision.other->HasComponent<CharacterController>())
		{
			mbIsPlay = true;

			for (const auto& track : mTracks)
				track->WakeUp();
		}
	}

	void Sequence::checkSeqeunce()
	{
		// 다른 시퀀스가 플레이 중일 때, 강제 종료
		if (mDurationTime == 0.f && mbIsStopOtherSequence)
		{
			for (auto& sequenceObject : GetScene()->GetComponentView<Sequence>())
			{
				auto sequence = sequenceObject.GetComponent<Sequence>();

				if (sequence != this && sequence->GetIsPlay())
					sequence->StopSequnce();
			}
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
		// UI 끄기
		if (mbIsOffUIRender)
			GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });

		// UI 켜기
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

	void Sequence::StopSequnce()
	{
		mDurationTime = FLT_MAX - 10.f;
	}
}
