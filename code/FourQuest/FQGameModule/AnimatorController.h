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
	/// 애니메이션의 상태를 컨트롤하는 클래스
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
		/// 애니메이터를 연결합니다 
		/// </summary>
		void SetAnimator(Animator* animator) { mAnimator = animator; }

		/// <summary>
		/// 애니메이션 상태를 업데이트합니다 
		/// </summary>
		void UpdateState(float dt);

		/// <summary>
		/// 애니메이션의 프레임을 업데이트합니다
		/// </summary>
		float UpdateAnimation(float dt);

		/// <summary>
		/// 파라미터 값을 설정합니다 
		/// </summary>
		void SetParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// 파라미터 값을 반환합니다 
		/// </summary>
		Parameter GetParameter(ParameterID id)const;

		/// <summary>
		/// 파라미터를 삭제합니다 
		/// </summary>
		void EraseParameter(ParameterID id);

		/// <summary>
		/// 파라미터를 추가합니다 
		/// </summary>
		void AddParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// 파타미터 팩을 반환합니다  
		/// </summary>
		ParameterPack GetParameterPack()const { return mParmeters; }

		/// <summary>
		/// 현재 애니메이션 이름을 반환합니다 
		/// </summary>
		StateName GetCurrentState() const { return mCurrentState->first; }

		/// <summary>
		/// 다음 애니메이션 이름을 반환합니다  
		/// </summary>
		StateName GetNextState() const { return mNextState->first; }

		/// <summary>
		/// 새로운 스테이트를 추가합니다 
		/// </summary>
		void CreateStateNode();

		/// <summary>
		/// 새로운 스테이트를 추가합니다 
		/// </summary>
		void AddStateNode(AnimationStateNode node);

		/// <summary>
		/// State를 담은 맵을 반환합니다
		/// </summary>
		StateMap& GetStateMap() { return mStates; }
		const StateMap& GetStateMap()const { return mStates; }

		/// <summary>
		/// 애니메이션 전환을 추가합니다 
		/// </summary>
		void AddTransition(StateName exit, StateName enter);

		/// <summary>
		/// 애니메이션 전환을 삭제합니다 
		/// </summary>
		void DeleteTransition(StateName exit, StateName enter);

		/// <summary>
		/// 애니메이션 상태를 삭제합니다 
		/// </summary>
		void DeleteState(StateName state);

		/// <summary>
		/// 스테이트의 이름을 변경합니다 
		/// </summary>
		bool ChangeStateName(StateName orginName, StateName changeName);

		std::vector<AnimationTransition>& GetTransitions() { return mTransitions; }
		const std::vector<AnimationTransition>& GetTransitions() const { return mTransitions; }

		/// <summary>
		/// 현재 애니메이션 재생 시간을 반환합니다
		/// </summary>
		float GetTimePos()const { return mTimePos; }

		/// <summary>
		/// 현재 블랜딩 애니메이션 재생 시간을 반환합니다 
		/// </summary>
		float GetBlendTimePos() const { return mBlendTimePos; }
		
		/// <summary>
		/// 애니메이션이 전환중인 상태인지 반환합니다.
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