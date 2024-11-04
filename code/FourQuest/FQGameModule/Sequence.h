#pragma once
#include "Component.h"

#include "SequenceStructure.h"

#include "Scene.h"
#include "Track.h"
#include "CameraChangeTrack.h"
#include "CameraShakeTrack.h"
#include "ObjectMoveTrack.h"
#include "ObjectTeleportTrack.h"
#include "PlayerMoveTrack.h"
#include "PlayerTeleportTrack.h"
#include "TextPrintTrack.h"
#include "EffectTrack.h"
#include "SoundTrack.h"
#include "ObjectAnimationTrack.h"
#include "VibrationTrack.h"
#include "FadeTrack.h"
#include "CreateDeleteTrack.h"

namespace fq::game_module
{
	class Sequence : public Component
	{
	public:
		Sequence();
		~Sequence();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;
		virtual void OnTriggerEnter(const Collision& collision) override;

		/// <summary>
		/// �������� ��� ������ �� �ɼ��Դϴ�.
		/// </summary>
		bool GetIsPlay() const { return mbIsPlay; }
		void SetIsPlay(bool isPlay) { mbIsPlay = isPlay; }
		bool GetIsLoop() const { return mbIsLoop; }
		void SetIsLoop(bool isLoop) { mbIsLoop = isLoop; }
		bool GetIsOnce() const { return mbIsOnce; }
		void SetIsOnce(bool isOnce) { mbIsOnce = isOnce; }
		bool GetIsTimeStop() const { return mbIsTimeStop; }
		void SetIsTimeStop(bool isTimeStop) { mbIsTimeStop = isTimeStop; }
		bool GetIsOtherSequenceStop() const { return mbIsStopOtherSequence; }
		void SetIsOtherSequenceStop(bool isStopOtherSequence) { mbIsStopOtherSequence = isStopOtherSequence; }
		bool GetIsOffUIRender() const { return mbIsOffUIRender; }
		void SetIsOffUIRender(bool isOffUIRender) { mbIsOffUIRender = isOffUIRender; }

		/// <summary>
		/// �ν����� â���� �ش� �ð��� �����Ǿ� �ִ� ȭ���� Ȯ���� �� �ֵ��� ���� �Լ��Դϴ�.
		/// </summary>
		float GetDurationTime() const { return mDurationTime; }
		void SetDurationTime(float durationTime);
		void StopSequnce();

		/// <summary>
		/// Ʈ�� ����ü ������ �����ͼ� Ʈ���� �����ϱ� ���� �����͵� �����Դϴ�.
		/// </summary>
		const std::vector<CameraChangeTrackInfo>& GetCameraChangeTrackInfo() const { return mCameraChangeTrackInfo; }
		void SetCameraChangeTrackInfo(const std::vector<CameraChangeTrackInfo>& info) { mCameraChangeTrackInfo = info; }
		const std::vector<ObjectMoveTrackInfo>& GetObjectMoveTrackInfo() const { return mObjectMoveTrackInfo; }
		void SetObjectMoveTrackInfo(const std::vector<ObjectMoveTrackInfo>& info) { mObjectMoveTrackInfo = info; }
		const std::vector<ObjectTeleportTrackInfo>& GetObjectTeleportTrackInfo() const { return mObjectTeleportTrackInfo; }
		void SetObjectTeleportTrackInfo(const std::vector<ObjectTeleportTrackInfo>& info) { mObjectTeleportTrackInfo = info; }
		const std::vector<PlayerMoveTrackInfo>& GetPlayerMoveTrackInfo() const { return mPlayerMoveTrackInfo; }
		void SetPlayerMoveTrackInfo(const std::vector<PlayerMoveTrackInfo>& info) { mPlayerMoveTrackInfo = info; }
		const std::vector<PlayerTeleportTrackInfo>& GetPlayerTeleportTrackInfo() const { return mPlayerTeleportTrackInfo; }
		void SetPlayerTeleportTrackInfo(const std::vector<PlayerTeleportTrackInfo>& info) { mPlayerTeleportTrackInfo = info; }
		const std::vector<TextPrintTrackInfo>& GetTextPrintTrackInfo() const { return mTextPrintTrackInfo; }
		void SetTextPrintTrackInfo(const std::vector<TextPrintTrackInfo>& info) { mTextPrintTrackInfo = info; }
		const std::vector<EffectTrackInfo>& GetEffectTrackInfo() const { return mEffectTrackInfo; }
		void SetEffectTrackInfo(const std::vector<EffectTrackInfo>& info) { mEffectTrackInfo = info; }
		const std::vector<SoundTrackInfo>& GetSoundTrackInfo() const { return mSoundTrackInfo; }
		void SetSoundTrackInfo(const std::vector<SoundTrackInfo>& info) { mSoundTrackInfo = info; }
		const std::vector<ObjectAnimationInfo>& GetObjectAnimationInfo() const { return mObjectAnimationInfo; }
		void SetObjectAnimationInfo(const std::vector<ObjectAnimationInfo>& info) { mObjectAnimationInfo = info; }
		const std::vector<CameraShakeTrackInfo>& GetCameraShakeTrackInfo() const { return mCameraShakeTrackInfo; }
		void SetCameraShakeTrackInfo(const std::vector<CameraShakeTrackInfo>& info) { mCameraShakeTrackInfo = info; }
		const std::vector<VibrationTrackInfo>& GetVibrationTrackInfo() const { return mVibrationTrackInfo; }
		void SetVibrationTrackInfo(const std::vector<VibrationTrackInfo>& info) { mVibrationTrackInfo = info; }
		const std::vector<FadeTrackInfo>& GetFadeTrackInfo() const { return mFadeTrackInfo; }
		void SetFadeTrackInfo(const std::vector<FadeTrackInfo>& info) { mFadeTrackInfo = info; }
		const std::vector<CreateDeleteTrackInfo>& GetCreateDeleteTrackInfo() const { return mCreateDeleteTrackInfo; }
		void SetCreateDeleteTrackInfo(const std::vector<CreateDeleteTrackInfo>& info) { mCreateDeleteTrackInfo = info; }

		std::vector<std::shared_ptr<Track>>& GetTrackContainer() { return mTracks; }
		std::unordered_map<std::string, std::vector<std::shared_ptr<fq::graphics::IAnimation>>>& GetAnimationContainer() { return mAnimationContainer; }

	private:
		/// <summary>
		/// �ٸ� �������� �������� �ֳ� üũ�ϴ� �Լ��Դϴ�.
		/// </summary>
		void checkSeqeunce();

		/// <summary>
		/// �������� ������ �ִ� Ʈ������ �����Ű�� �Լ��Դϴ�.
		/// </summary>
		void playTrack(float dt);

		/// <summary>
		/// �ð� ���� �ɼ��� üũ���� ��� �������� ������ �ִ� ������Ʈ���� ���� Update�� �����ִ� �Լ��Դϴ�.
		/// </summary>
		void updateSequenceObject(float dt);

		/// <summary>
		/// Ʈ���� �����ϴ� ���ø� �Լ��Դϴ�.
		/// </summary>
		template<class ClassName, class TrackInfo, typename... Args>
		void createTrack(const TrackInfo& trackInfoContainer, Args&... arg);

		virtual entt::meta_handle GetHandle() override;
		virtual std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;

	private:
		std::vector<std::shared_ptr<Track>> mTracks;
		bool mbIsPlay;
		bool mbIsLoop;
		bool mbIsOnce;
		bool mbIsTimeStop;
		bool mbIsOffUIRender;
		bool mbIsStopOtherSequence;

		float mTotalPlayTime;
		float mDurationTime;

		std::vector<CameraChangeTrackInfo>		mCameraChangeTrackInfo;
		std::vector<ObjectMoveTrackInfo>		mObjectMoveTrackInfo;
		std::vector<ObjectTeleportTrackInfo>	mObjectTeleportTrackInfo;
		std::vector<PlayerMoveTrackInfo>		mPlayerMoveTrackInfo;
		std::vector<PlayerTeleportTrackInfo>	mPlayerTeleportTrackInfo;
		std::vector<ObjectAnimationInfo>		mObjectAnimationInfo;
		std::vector<EffectTrackInfo>			mEffectTrackInfo;
		std::vector<SoundTrackInfo>				mSoundTrackInfo;
		std::vector<TextPrintTrackInfo>			mTextPrintTrackInfo;
		std::vector<CameraShakeTrackInfo>		mCameraShakeTrackInfo;
		std::vector<VibrationTrackInfo>			mVibrationTrackInfo;
		std::vector<FadeTrackInfo>				mFadeTrackInfo;
		std::vector<CreateDeleteTrackInfo>		mCreateDeleteTrackInfo;
		std::vector<PrefabResource>				mPrefabs;

		std::unordered_map<std::string, std::vector<std::shared_ptr<fq::graphics::IAnimation>>> mAnimationContainer;

		bool mbIsProcessedUIRender;
			
		enum { DELAY_FRAME = 2 };
		unsigned int mUIRenderDelayFrame;
		unsigned int mUIRenderAccumlateFrame;

		friend void RegisterMetaData();
	};

	template<typename ClassName, typename TrackInfo, typename... Args>
	inline void Sequence::createTrack(const TrackInfo& trackInfoContainer, Args&... arg)
	{
		auto scene = GetScene();
		bool check = false;

		for (const auto& trackInfo : trackInfoContainer)
		{
			// Ʈ�� ����
			std::shared_ptr<ClassName> track = std::make_shared<ClassName>();

			// Ʈ�� �ʱ�ȭ
			check = track->Initialize(trackInfo, scene, arg...);

			if (check)
			{
				// Ʈ���� ��Ͽ� �߰�
				mTracks.push_back(track);

				// �� ��� �ð� ���
				float trackTotalTime = track->GetStartTime() + track->GetTotalPlayTime();

				// �� ��� �ð� ����
				if (mTotalPlayTime < trackTotalTime)
					mTotalPlayTime = trackTotalTime;
			}
			else
			{
				spdlog::error("[Sequence ({})] Failed Create Track", __LINE__);
			}
		}
	}
}