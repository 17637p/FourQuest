#pragma once
#pragma once

#include <unordered_map>
#include <string>

#include "../FQReflect/entt.hpp"
#include "AnimationStateNode.h"
#include "AnimationTransition.h"

namespace fq::game_module
{
	class Animator;

	/// <summary>
	/// �ִϸ��̼��� ���¸� ��Ʈ���ϴ� Ŭ����
	/// </summary>
	class AnimatorController
	{
		using ParameterID = std::string;
		using Parameter = entt::meta_any;
		using ParameterPack = std::unordered_map<ParameterID, Parameter>;

		using StateName = std::string;
		using StateMap = std::unordered_map<StateName, AnimationStateNode>;
		using StateIterator = StateMap::iterator;

		using TransitionMap = std::unordered_multimap<StateName, AnimationTransition>;
		using TransitionIterator = TransitionMap::iterator;

	public:
		AnimatorController();
		~AnimatorController();

		/// <summary>
		/// �ִϸ��̼� ������Ʈ ������Ʈ ȣ��
		/// </summary>
		void Update(float dt);

		/// <summary>
		/// �ִϸ����͸� ��ȯ�մϴ�
		/// </summary>
		Animator* GetAnimator()const { return mAnimator; }

		/// <summary>
		/// �ִϸ����͸� �����մϴ� 
		/// </summary>
		void SetAnimator(Animator* animator) { mAnimator = animator; }

		/// <summary>
		/// �ִϸ��̼� ���¸� ������Ʈ�մϴ� 
		/// </summary>
		void UpdateState(float dt);

		/// <summary>
		/// �ִϸ��̼��� �������� ������Ʈ�մϴ�
		/// </summary>
		void UpdateAnimation(float dt);

		/// <summary>
		/// �Ķ���� ���� �����մϴ� 
		/// </summary>
		void SetParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// �Ķ���� ���� ��ȯ�մϴ� 
		/// </summary>
		Parameter GetParameter(ParameterID id)const;

		/// <summary>
		/// �Ķ���͸� �����մϴ� 
		/// </summary>
		void EraseParameter(ParameterID id);

		/// <summary>
		/// �Ķ���͸� �߰��մϴ� 
		/// </summary>
		void AddParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// ��Ÿ���� ���� ��ȯ�մϴ�  
		/// </summary>
		ParameterPack GetParameterPack()const { return mParmeters; }

		/// <summary>
		/// ���� �ִϸ��̼� �̸��� ��ȯ�մϴ� 
		/// </summary>
		StateName GetCurrentStateName() const { return mCurrentState->first; }

		/// <summary>
		/// ���� �ִϸ��̼��� �ִϸ��̼� �������̽��� ��ȯ�մϴ� 
		/// </summary>
		fq::graphics::IAnimation& GetCurrentStateAnimation() const { return mCurrentState->second.GetAnimation(); }
		std::shared_ptr<fq::graphics::IAnimation> GetSharedCurrentStateAnimation() const { return mCurrentState->second.GetSharedAnimation(); }
		const std::shared_ptr<fq::graphics::IAnimation>& GetSharedRefCurrentStateAnimation() const { return mCurrentState->second.GetSharedRefAnimation(); }
		bool GetHasCurrentStateAnimation() const { return mCurrentState->second.GetHasAnimation(); }

		/// <summary>
		/// ���� �ִϸ��̼� �̸��� ��ȯ�մϴ�  
		/// </summary>
		StateName GetNextStateName()const;

		/// <summary>
		/// ���� �ִϸ��̼��� �ִϸ��̼� �������̽��� ��ȯ�մϴ�
		/// </summary>
		fq::graphics::IAnimation& GetNextStateAnimation() const;
		std::shared_ptr<fq::graphics::IAnimation> GetSharedNextStateAnimationOrNull() const;
		const std::shared_ptr<fq::graphics::IAnimation>& GetSharedRefNextStateAnimation() const;
		bool GetHasNextStateAnimation() const;

		/// <summary>
		/// ���ο� ������Ʈ�� �߰��մϴ� 
		/// </summary>
		void CreateStateNode();

		/// <summary>
		/// ���ο� ������Ʈ�� �߰��մϴ� 
		/// </summary>
		void AddStateNode(AnimationStateNode node);

		/// <summary>
		/// State�� ���� ���� ��ȯ�մϴ�
		/// </summary>
		StateMap& GetStateMap() { return mStates; }
		const StateMap& GetStateMap()const { return mStates; }

		/// <summary>
		/// �ִϸ��̼� ��ȯ�� �߰��մϴ� 
		/// </summary>
		void AddTransition(AnimationTransition transition);

		/// <summary>
		/// �ִϸ��̼� ��ȯ�� �����մϴ� 
		/// </summary>
		void DeleteTransition(StateName exit, StateName enter);

		/// <summary>
		/// �ִϸ��̼� ���¸� �����մϴ� 
		/// </summary>
		void DeleteState(StateName state);

		/// <summary>
		/// ������Ʈ�� �̸��� �����մϴ� 
		/// </summary>
		bool ChangeStateName(StateName orginName, StateName changeName);

		TransitionMap& GetTransitionMap() { return mTransitions; }
		const TransitionMap& GetTransitionMap() const { return mTransitions; }

		/// <summary>
		/// ���� �ִϸ��̼� ��� �ð��� ��ȯ�մϴ�
		/// </summary>
		float GetTimePos()const { return mTimePos; }

		/// <summary>
		/// ���� ���� �ִϸ��̼� ��� �ð��� ��ȯ�մϴ� 
		/// </summary>
		float GetBlendTimePos() const { return mBlendTimePos; }

		/// <summary>
		/// �ִϸ��̼��� ��ȯ���� �������� ��ȯ�մϴ�.
		/// </summary>
		bool IsInTransition()const;

		/// <summary>
		/// ���� ����Ʈ�� ��ȯ�մϴ�.
		/// </summary>
		float GetBlendWeight()const;

		TransitionIterator GetCurrentTransition();

	private:
		bool checkNextStateTransition();
		bool checkCurrentStateTransition();

		TransitionIterator checkStateTransition(const StateName& name);
		bool checkConditions(const AnimationTransition& transition, float timePos);
		void emitChangeState();

	public:
		static constexpr char OnTrigger = static_cast<char>(true);
		static constexpr char OffTrigger = static_cast<char>(false);
		static constexpr float EndTransitionWeight = 1.f;
	private:
		Animator* mAnimator;

		ParameterPack mParmeters;
		StateMap mStates;
		TransitionMap mTransitions;

		TransitionIterator mCurrentTransition;
		StateIterator mCurrentState;
		StateIterator mNextState;

		float mTimePos;
		float mBlendTimePos;
		float mBlendWeight;
		float mBlendElapsedTime;


		// animation ref Map
		// animation cache
	};
}