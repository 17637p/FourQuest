#pragma once

#include <set>
#include <vector>
#include <string>

#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class Track;

	struct TrackKey
	{
		float time = 0.f;
		DirectX::SimpleMath::Vector3 position = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 rotation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 scale = { 1.f, 1.f, 1.f };
	};

	struct AnimationTrackKey
	{
		float time = 0.f;
		std::string animationPath = {};
	};

	struct CameraChangeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetCameraName = {};
		std::string prevCameraName = {};
		std::vector<TrackKey> keys;
	};

	struct ObjectMoveTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<TrackKey> keys;
	};

	struct ObjectTeleportTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<TrackKey> keys;
	};

	struct ObjectAnimationInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<AnimationTrackKey> animationTrackKeys = {};
	};

	struct EffectTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string effectPath = {};
		std::vector<TrackKey> keys;
	};

	struct SoundTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string keyName = {};
		std::string soundObjectName = {};
		bool bIsLoop = false;
	};
	
	struct TextPrintTrackInfo
	{
		std::string fontPath = {};// "던파 연단된 칼날";
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		float nameFontCenterY = 200.f;
		float nameFontSize = 30.f;
		DirectX::SimpleMath::Color nameFontColor = { 0.f, 0.f, 1.f, 1.f };
		std::string name = {};

		float textFontCenterY = 150.f;
		float textFontSize = 20.f;
		DirectX::SimpleMath::Color textFontColor = { 1.f, 1.f, 1.f, 1.f };
		std::string text = {};
	};

	struct CameraShakeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string cameraObjectName = {};
		DirectX::SimpleMath::Vector3 magnitude = {};
		int shakeCount = 100;
		int originInitSpacing = 5;
	};

	class Sequence : public Component
	{
	public:
		Sequence();
		~Sequence();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnTriggerEnter(const Collision& collision) override;

		bool GetIsPlay() const { return mbIsPlay; }
		void SetIsPlay(bool isPlay) { mbIsPlay = isPlay; }
		bool GetIsLoop() const { return mbIsLoop; }
		void SetIsLoop(bool isLoop) { mbIsLoop = isLoop; }
		bool GetIsOnce() const { return mbIsOnce; }
		void SetIsOnce(bool isOnce) { mbIsOnce = isOnce; }

		/// <summary>
		/// 인스펙터 창에서 해당 시간에 정지되어 있는 화면을 확인할 수 있도록 만든 함수입니다.
		/// </summary>
		float GetDurationTime() const { return mDurationTime; }
		void SetDurationTime(float durationTime);

		const std::vector<CameraChangeTrackInfo>& GetCameraChangeTrackInfo() const { return mCameraChangeTrackInfo; }
		void SetCameraChangeTrackInfo(const std::vector<CameraChangeTrackInfo>& info) { mCameraChangeTrackInfo = info; }
		const std::vector<ObjectMoveTrackInfo>& GetObjectMoveTrackInfo() const { return mObjectMoveTrackInfo; }
		void SetObjectMoveTrackInfo(const std::vector<ObjectMoveTrackInfo>& info) { mObjectMoveTrackInfo = info; }
		const std::vector<ObjectTeleportTrackInfo>& GetObjectTeleportTrackInfo() const { return mObjectTeleportTrackInfo; }
		void SetObjectTeleportTrackInfo(const std::vector<ObjectTeleportTrackInfo>& info) { mObjectTeleportTrackInfo = info; }
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

		std::vector<std::shared_ptr<Track>>& GetTrackContainer() { return mTracks; }
		std::unordered_map<std::string, std::vector<std::shared_ptr<fq::graphics::IAnimation>>>& GetAnimationContainer() { return mAnimationContainer; }
		  
	private:
		virtual entt::meta_handle GetHandle() override;
		virtual std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;

	private:
		std::vector<std::shared_ptr<Track>> mTracks;
		bool mbIsPlay;
		bool mbIsLoop;
		bool mbIsOnce;

		float mTotalPlayTime;
		float mDurationTime;

		std::vector<CameraChangeTrackInfo>		mCameraChangeTrackInfo;
		std::vector<ObjectMoveTrackInfo>		mObjectMoveTrackInfo;
		std::vector<ObjectTeleportTrackInfo>	mObjectTeleportTrackInfo;
		std::vector<ObjectAnimationInfo>		mObjectAnimationInfo;
		std::vector<EffectTrackInfo>			mEffectTrackInfo;
		std::vector<SoundTrackInfo>				mSoundTrackInfo;
		std::vector<TextPrintTrackInfo>			mTextPrintTrackInfo;
		std::vector<CameraShakeTrackInfo>		mCameraShakeTrackInfo;

		std::unordered_map<std::string, std::vector<std::shared_ptr<fq::graphics::IAnimation>>> mAnimationContainer;
	};
}