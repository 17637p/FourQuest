#pragma once

#include "Component.h"

namespace fq::game_module
{
	enum class ETrackType
	{
		SCRIPT = 0,			// 대사 출력
		TIME_STOP,			// 시간 멈춤
		TIME_START,			// 시간 재생
		CAMERA_CHANGE,		// 카메라 변경
		OBJECT_TELEPORT,	// 오브젝트 순간이동
		OBJECT_MOVE,		// 오브젝트 이동
		TIME_WAIT,			// 시간 대기

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

	class Track
	{
	public:
		Track(ETrackType type);
		virtual ~Track() {}

		ETrackState Play(float deltaTime);

		const ETrackType& GetType() { return mType; }
		const ETrackState& GetCurrentState() { return mCurrentState; }
		const float& GetStartTime() { return mStartTime; }
		const float& GetTotalPlayTime() { return mTotalPlayTime; }
		const float& GetElapsedTime() { return mElapsedTime; }
		void SetStartTime(const float& startTime) { mStartTime = startTime; }
		void SetTotalPlayTime(const float& playTime) { mTotalPlayTime = playTime; }

	protected:
		virtual void PlayEnter() abstract;
		virtual void PlayOn() abstract;
		virtual void PlayExit() abstract;

	private:
		void handleState();
		void nodeUpdate();

	protected:
		ETrackType mType;
		ETrackState mCurrentState;

		float mStartTime;
		float mTotalPlayTime;
		float mElapsedTime;
	};
}

