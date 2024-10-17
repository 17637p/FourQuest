#pragma once

#include "Component.h"

namespace fq::game_module
{
	enum class ETrackType
	{
		TEXT_PRINT = 0,		// 대사 출력
		TIME_STOP,			// 시간 멈춤
		TIME_START,			// 시간 재생
		CAMERA_CHANGE,		// 카메라 변경
		OBJECT_TELEPORT,	// 오브젝트 순간이동
		OBJECT_MOVE,		// 오브젝트 이동
		ANIMAITON,			// 애니메이션
		EFFECT,				// 이펙트
		SOUND,				// 사운드
		CAMERA_SHAKE,		// 카메라 진동
		VIBRATION,			// 컨트롤러 진동
		PLAYER_TELEPORT,	// 플레이어 순간이동
		PLAYER_MOVE,		// 플레이어 이동

		END
	};

	enum class ETrackState
	{
		OFF = 0,			// 꺼짐
		ENTER,				// 시작 
		ON,					// 진행중
		EXIT,				// 끝

		END
	};

	class Scene;

	class Track
	{
	public:
		Track(ETrackType type);
		virtual ~Track() {}

		ETrackState Play(float deltaTime);
		virtual void End() abstract;
		void WakeUp();

		const std::vector<std::string>& GetTrackObjectName() { return mTrackObjectName; }
		const ETrackType& GetType() { return mType; }
		const ETrackState& GetCurrentState() { return mCurrentState; }
		const float& GetStartTime() { return mStartTime; }
		const float& GetTotalPlayTime() { return mTotalPlayTime; }
		const float& GetElapsedTime() { return mElapsedTime; }
		void SetStartTime(const float& startTime) { mStartTime = startTime; }
		void SetTotalPlayTime(const float& playTime) { mTotalPlayTime = playTime; }

	protected:
		DirectX::SimpleMath::Vector3 LerpVector3(const DirectX::SimpleMath::Vector3& a, const DirectX::SimpleMath::Vector3& b, float t);

		virtual void PlayEnter() abstract;
		virtual void PlayOn() abstract;
		virtual void PlayExit() abstract;

	private:
		void handleState();
		void nodeUpdate();

	protected:
		std::vector<std::string> mTrackObjectName;
		ETrackType mType;
		ETrackState mCurrentState;

		DirectX::SimpleMath::Vector3 mCurPosition;
		DirectX::SimpleMath::Vector3 mPrevPosition;

		float mStartTime;
		float mTotalPlayTime;
		float mElapsedTime;
	};
}

