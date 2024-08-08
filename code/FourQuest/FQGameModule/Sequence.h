#pragma once

#include <set>
#include <vector>
#include <string>

#include "Component.h"

namespace fq::game_module
{
	class Track;

	struct CameraChangeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetCameraName = {};
		std::string prevCameraName = {};
	};

	struct ObjectMoveTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		DirectX::SimpleMath::Vector3 targetPosition = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 targetRotation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 targetScale	= { 1.f, 1.f, 1.f };
	};

	struct ObjectTeleportTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		DirectX::SimpleMath::Vector3 targetPosition= { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 targetRotation= { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 targetScale = { 1.f, 1.f, 1.f };
	};

	struct ObjectAnimationInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
	};

	struct EffectTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;


	};
	
	struct TextPrintTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		float nameFontCenterY = 100.f;
		float nameFontSize = 12.f;
		DirectX::SimpleMath::Color nameFontColor = { 0.f, 0.f, 1.f, 1.f };
		std::string name = {};

		float textFontCenterY = 150.f;
		float textFontSize = 10.f;
		DirectX::SimpleMath::Color textFontColor = { 1.f, 1.f, 1.f, 1.f };
		std::string text = {};
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

		const std::vector<CameraChangeTrackInfo>& GetCameraChangeTrackInfo() const { return mCameraChangeTrackInfo; }
		void SetCameraChangeTrackInfo(const std::vector<CameraChangeTrackInfo>& info) { mCameraChangeTrackInfo = info; }
		const std::vector<ObjectMoveTrackInfo>& GetObjectMoveTrackInfo() const { return mObjectMoveTrackInfo; }
		void SetObjectMoveTrackInfo(const std::vector<ObjectMoveTrackInfo>& info) { mObjectMoveTrackInfo = info; }
		const std::vector<ObjectTeleportTrackInfo>& GetObjectTeleportTrackInfo() const { return mObjectTeleportTrackInfo; }
		void SetObjectTeleportTrackInfo(const std::vector<ObjectTeleportTrackInfo>& info) { mObjectTeleportTrackInfo = info; }
		const std::vector<TextPrintTrackInfo>& GetTextPrintTrackInfo() const { return mTextPrintTrackInfo; }
		void SetTextPrintTrackInfo(const std::vector<TextPrintTrackInfo>& info) { mTextPrintTrackInfo = info; }
		  
	private:
		virtual entt::meta_handle GetHandle() override;
		virtual std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;

	private:
		std::vector<std::shared_ptr<Track>> mTracks;
		bool mbIsPlay;
		bool mbIsLoop;

		float mTotalPlayTime;
		float mDurationTime;

		std::vector<CameraChangeTrackInfo> mCameraChangeTrackInfo;
		std::vector<ObjectMoveTrackInfo> mObjectMoveTrackInfo;
		std::vector<ObjectTeleportTrackInfo> mObjectTeleportTrackInfo;
		std::vector<ObjectAnimationInfo> mObjectAnimationInfo;
		std::vector<EffectTrackInfo> mEffectTrackInfo;
		std::vector<TextPrintTrackInfo> mTextPrintTrackInfo;
	};


}