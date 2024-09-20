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
	/// �ִϸ��̼��� ���¸� ��Ÿ���� ���
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

		struct Event
		{
			std::string FunctionName;
			float Time; // �ش�ð��� ���� ��� ����Ʈ �̺�Ʈ�� �߻���Ŵ
			bool bIsProcessed; // ����Ʈ �̺�Ʈ �߻� ����
			bool bIsFired; // ����Ʈ �̺�Ʈ �߻� ����
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
		std::string GetAnimationPath() const { return mAnimationPath; }
		void SetAnimationPath(std::string val) { mAnimationPath = val; }
		std::string GetAnimationKey() const { return mAnimationKey; }
		void SetAnimationKey(std::string val) { mAnimationKey = val; }
		float GetDuration() const { return mDuration; }
		void SetDuration(float val) { mDuration = val; }
		bool IsLoof() const { return mbIsLoof; }
		void SetLoof(bool val) { mbIsLoof = val; }
		float GetStartTimePos() const { return mStartTimePos; }
		void SetStartTimePos(float val) { mStartTimePos = val; }
		std::vector<Event>& GetEvents() { return mEvents; }
		const std::vector<Event>& GetEvents() const { return mEvents; }
		void SetEvents(const std::vector<Event>& events) { mEvents = events; }

		void ProcessAnimationEvent(class GameObject* gameObject, class EventManager* eventManager);

	public:
		// �ִϸ��̼� ���ҽ� ����
		void SetAnimation(std::shared_ptr<fq::graphics::IAnimation> animation) { mAnimation = animation; }
		fq::graphics::IAnimation& GetAnimation() { return *mAnimation; }
		std::shared_ptr<fq::graphics::IAnimation> GetSharedAnimation() { return mAnimation; }
		const std::shared_ptr<fq::graphics::IAnimation>& GetSharedRefAnimation() { return mAnimation; }
		bool GetHasAnimation() { return mAnimation != nullptr; }

		StateBehaviourMap& GetStateBehaviourMap() { return mBehaviours; }
		const StateBehaviourMap& GetStateBehaviourMap() const { return mBehaviours; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		AnimatorController* mController;
		Type mType;
		std::string mAnimationPath;
		std::string mAnimationKey;
		float mPlayBackSpeed;
		float mStartTimePos;
		float mDuration;
		bool mbIsLoof;

		StateBehaviourMap mBehaviours;
		std::shared_ptr<fq::graphics::IAnimation> mAnimation;

		// ���ʿ� ó���� �̺�Ʈ ��� �߰�
		std::vector<Event> mEvents;
		float mAccumulationTime;

		friend void RegisterMetaData();
	};

}