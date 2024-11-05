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
		, mbIsProcessedUIRender(false)
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
		, mPrefabs{}
		, mUIRenderDelayFrame(DELAY_FRAME)
		, mUIRenderAccumlateFrame(0)
	{
	}

	Sequence::~Sequence()
	{

	}

	void Sequence::OnStart()
	{
		mTracks.clear();

		// Ʈ�� ����
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
		createTrack<FadeTrack>(mFadeTrackInfo);
		createTrack<CreateDeleteTrack>(mCreateDeleteTrackInfo, mPrefabs);
		createTrack<LightTrack>(mLightTrackInfo);

		auto scene = GetScene();
		bool check = false;
		mbIsProcessedUIRender = false;

		// �ִϸ��̼� Ʈ�� ����
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

		mUIRenderDelayFrame = DELAY_FRAME;
		mUIRenderAccumlateFrame = 0;
	}

	void Sequence::OnUpdate(float dt)
	{
		if (mbIsPlay)
		{
			++mUIRenderAccumlateFrame;

			if (!mbIsProcessedUIRender && mbIsOffUIRender && mUIRenderDelayFrame < mUIRenderAccumlateFrame)
			{
				GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });
				mbIsProcessedUIRender = true;
			}

			checkSeqeunce();

			float deltaTime = GetScene()->GetTimeManager()->GetDeltaTime();
			mDurationTime += deltaTime;

			playTrack(deltaTime);
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
					spdlog::trace("sequence {} delete", GetGameObject()->GetName());
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
		if (mbIsOffUIRender)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });
		}

		auto input = GetScene()->GetInputManager();

		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			input->SetVibration(i, EVibrationMode::Both, 0.f, mTotalPlayTime);
		}
	}

	void Sequence::OnTriggerEnter(const Collision& collision)
	{
		if (collision.other->HasComponent<CharacterController>() && !mbIsPlay)
		{
			mbIsPlay = true;

			for (const auto& track : mTracks)
				track->WakeUp();
		}
	}

	void Sequence::checkSeqeunce()
	{
		// �ٸ� �������� �÷��� ���� ��, ���� ����
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
		// �������� ������ �ִ� Ʈ���� Play
		for (const auto& track : mTracks)
		{
			if (dt == 0.f && track->GetType() == ETrackType::TEXT_PRINT)
				continue;

			track->Play(mDurationTime);
		}

		// ���� �ð��� ����Ǿ����� Ʈ���� End
		if (mDurationTime >= mTotalPlayTime)
		{
			for (const auto& track : mTracks)
			{
				track->WakeUp();
				track->End();
			}
		}
	}

	void Sequence::updateSequenceObject(float dt)
	{
		if (!mbIsTimeStop)
			return;

		GetScene()->GetTimeManager()->SetTimeScale(0.f);

		// �ð� ���� ���� ��쿡 �������� ������ �ִ� ������Ʈ ������Ʈ ( ��, �ؽ�ƮUI�� ����(���� ��) )
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

	void Sequence::StopSequnce()
	{
		mDurationTime = FLT_MAX - 10.f;
	}
}
