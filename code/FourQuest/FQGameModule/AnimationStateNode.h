#pragma once

#include <memory>
#include <unordered_map>

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQReflect/FQReflect.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class AnimatorController;
	class IStateBehaviour;

	/// <summary>
	/// 애니메이션의 상태를 나타내는 노드
	/// </summary>
	class AnimationStateNode : public fq::reflect::IHandle
	{
		using StateBehaviourMap = std::unordered_map<entt::id_type, std::shared_ptr<IStateBehaviour>>;

	public:
		enum class Type
		{
			Entry,
			Exit,
			State,
			AnyState,
		};

	public:
		AnimationStateNode(AnimatorController* controller);
		~AnimationStateNode();

		void OnStateEnter();
		void OnStateExit();
		void OnStateUpdate(float dt);

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
		float GetDuration() const { return mDuration; }
		void SetDuration(float val) { mDuration = val; }
		bool IsLoof() const { return mbIsLoof; }
		void SetLoof(bool val) { mbIsLoof = val; }
		void SetAnimation(std::shared_ptr<fq::graphics::IAnimation> animation) { mAnimation = animation; }
		std::shared_ptr<fq::graphics::IAnimation> GetAnimation() { return mAnimation; }

		StateBehaviourMap& GetStateBehaviourMap() { return mBehaviours; }
		const StateBehaviourMap& GetStateBehaviourMap() const { return mBehaviours; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		AnimatorController* mController;
		Type mType;
		std::string mModelPath;
		std::string mAnimationName;
		std::string mAnimationKey;
		float mPlayBackSpeed;
		float mDuration;
		bool mbIsLoof;

		StateBehaviourMap mBehaviours;
		std::shared_ptr<fq::graphics::IAnimation> mAnimation;
	};

}