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

	public:
		AnimatorController();
		~AnimatorController();

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
		float UpdateAnimation(float dt);

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
		StateName GetCurrentState() const { return mCurrentState->first; }

		/// <summary>
		/// ���� �ִϸ��̼� �̸��� ��ȯ�մϴ�  
		/// </summary>
		StateName GetNextState() const { return mNextState->first; }

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
		void AddTransition(StateName exit, StateName enter);

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

		std::vector<AnimationTransition>& GetTransitions() { return mTransitions; }
		const std::vector<AnimationTransition>& GetTransitions() const { return mTransitions; }

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

	private:
		bool checkConditions(AnimationTransition& transition);

	public:
		static constexpr char OnTrigger = static_cast<char>(true);
		static constexpr char OffTrigger = static_cast<char>(false);

	private:
		Animator* mAnimator;

		ParameterPack mParmeters;
		StateMap mStates;
		std::vector<AnimationTransition> mTransitions;

		StateIterator mCurrentState;
		StateIterator mNextState;

		float mTimePos;
		float mBlendTimePos;
		float mBlendWeight;
		
	};

}