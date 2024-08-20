#pragma once

#include "Track.h"
#include "Sequence.h"

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
		std::string mCameraObjectName;

		float mPrevTime;
		float mCurrentTime;
		float mDurationTime;
		
		DirectX::SimpleMath::Vector3 mMagnitude;
		DirectX::SimpleMath::Vector3 mTargetPosition;
		DirectX::SimpleMath::Vector3 mOrizinPosition;
		DirectX::SimpleMath::Vector3 mCurrentPoisiton;
		DirectX::SimpleMath::Vector3 mPrevPoisiton;

		int mOriginInitSpacing;
		int mCurrentShakeCount;
		int mShakeCount;

		GameObject* mCameraObject;
		Scene* mScene;
	};
}