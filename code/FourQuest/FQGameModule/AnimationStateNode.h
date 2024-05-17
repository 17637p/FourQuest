#pragma once

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class AnimatorController;

	/// <summary>
	/// 애니메이션의 상태를 나타내는 노드
	/// </summary>
	class AnimationStateNode : public fq::reflect::IHandle
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

		float GetPlayBackSpeed() const { return mPlayBackSpeed; }
		void SetPlayBackSpeed(float speed) { mPlayBackSpeed = speed; }

		std::string GetModelPath() const { return mModelPath; }
		void SetModelPath(std::string val) { mModelPath = val; }
		std::string GetAnimationName() const { return mAnimationName; }
		void SetAnimationName(std::string val) { mAnimationName = val; }
		std::string GetAnimationKey() const { return mAnimationKey; }
		void SetAnimationKey(std::string val) { mAnimationKey = val; }
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		AnimatorController* mController;
		Type mType;
		std::string mModelPath;
		std::string mAnimationName;
		std::string mAnimationKey;
		float mPlayBackSpeed;
	};

}