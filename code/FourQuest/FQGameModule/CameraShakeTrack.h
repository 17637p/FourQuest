#pragma once

#include "Track.h"
#include "SequenceStructure.h"

namespace fq::game_module
{
	class CameraShakeTrack : public Track
	{
	public:
		CameraShakeTrack();
		~CameraShakeTrack();

		bool Initialize(const CameraShakeTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		void GenerateNewShakeOffset();

	private:
		std::string mCameraObjectName;

		float mPrevTime;
		float mCurrentTime;
		float mDurationTime;
		
		DirectX::SimpleMath::Vector3 mMagnitude;
		DirectX::SimpleMath::Vector3 mTargetPosition;
		DirectX::SimpleMath::Vector3 mCameraOffsetPosition;
		DirectX::SimpleMath::Vector3 mOffsetNextPosition;
		DirectX::SimpleMath::Vector3 mOffsetPrevPosition;
		DirectX::SimpleMath::Vector3 mPrevPosition;

		int mOriginInitSpacing;
		int mCurrentShakeCount;
		int mShakeCount;

		GameObject* mCameraObject;
		Scene* mScene;
	};
}