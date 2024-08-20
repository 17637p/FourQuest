#pragma once

#include "Component.h"

namespace fq::game_module
{
	enum class ETrackType
	{
		TEXT_PRINT = 0,		// ��� ���
		TIME_STOP,			// �ð� ����
		TIME_START,			// �ð� ���
		CAMERA_CHANGE,		// ī�޶� ����
		OBJECT_TELEPORT,	// ������Ʈ �����̵�
		OBJECT_MOVE,		// ������Ʈ �̵�
		ANIMAITON,			// �ִϸ��̼�
		EFFECT,				// ����Ʈ
		SOUND,				// ����
		CAMERA_SHAKE,		// ī�޶� ����

		END
	};

	enum class ETrackState
	{
		OFF = 0,			// ����
		ENTER,				// ���� 
		ON,					// ������
		EXIT,				// ��

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
		ETrackType mType;
		ETrackState mCurrentState;

		float mStartTime;
		float mTotalPlayTime;
		float mElapsedTime;
	};
}

