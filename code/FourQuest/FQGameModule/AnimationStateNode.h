#pragma once

#include "../FQCommon/FQCommonGraphics.h"


namespace fq::game_module
{
	class AnimatorController;

	/// <summary>
	/// 애니메이션의 상태를 나타내는 노드
	/// </summary>
	class AnimationStateNode
	{
	public:
		enum class Type
		{
			Entry, 
			Exit,
			State,
			AnyState, 
		};

		AnimationStateNode(AnimatorController* controller);
		~AnimationStateNode();

		Type GetType() const { return mType; }
		void SetType(Type val) { mType = val; }

		fq::graphics::AnimationInfo GetAniInfo() const { return mAniInfo; }
		void SetAniInfo(fq::graphics::AnimationInfo info) { mAniInfo = info; }

		float GetPlayBackSpeed() const { return mPlayBackSpeed; }
		void SetPlayBackSpeed(float speed) { mPlayBackSpeed = speed; }

	private:
		AnimatorController* mController;
		Type mType;
		fq::graphics::AnimationInfo mAniInfo;
		float mPlayBackSpeed;
	};

}