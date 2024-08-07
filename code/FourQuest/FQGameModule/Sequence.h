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
		float startTime;
		float totalPlayTime;

		std::string targetCameraName;
		std::string prevCameraName;
	};

	struct ObjectMoveTrackInfo
	{
		float startTime;
		float totalPlayTime;

		std::string targetObjectName;
		DirectX::SimpleMath::Vector3 targetPosition;
		DirectX::SimpleMath::Vector3 targetRotation;
		DirectX::SimpleMath::Vector3 targetScale;
	};

	struct ObjectTeleportTrackInfo
	{
		float startTime;
		float totalPlayTime;

		std::string targetObjectName;
		DirectX::SimpleMath::Vector3 targetPosition;
		DirectX::SimpleMath::Vector3 targetRotation;
		DirectX::SimpleMath::Vector3 targetScale;
	};

	class Sequence : public Component
	{
	public:
		Sequence();
		~Sequence();
		
		virtual void OnUpdate(float dt) override;
		virtual void OnTriggerEnter(const Collision& collision) override;

		bool GetIsPlay() { return mbIsPlay; }
		void SetIsPlay(const bool& isPlay) { mbIsPlay = isPlay; }
		bool GetIsLoop() { return mbIsLoop; }
		void SetIsLoop(const bool& isLoop) { mbIsLoop = isLoop; }

		const std::vector<CameraChangeTrackInfo>& GetCameraChangeTrackInfo() { return mCameraChangeTrackInfo; }
		void SetCameraChangeTrackInfo(const std::vector<CameraChangeTrackInfo>& info) { mCameraChangeTrackInfo = info; }
		const std::vector<ObjectMoveTrackInfo>& GetObjectMoveTrackInfo() { return mObjectMoveTrackInfo; }
		void SetObjectMoveTrackInfo(const std::vector<ObjectMoveTrackInfo>& info) { mObjectMoveTrackInfo = info; }
		const std::vector<ObjectTeleportTrackInfo>& GetObjectTeleportTrackInfo() { return mObjectTeleportTrackInfo; }
		void SetObjectTeleportTrackInfo(const std::vector<ObjectTeleportTrackInfo>& info) { mObjectTeleportTrackInfo = info; }

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
	};


}