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

		using TransitionMap = std::unordered_multimap<StateName, AnimationTransition>;
		using TransitionIterator = TransitionMap::iterator;

	public:
		AnimatorController();
		~AnimatorController();

		/// <summary>
		/// 애니메이션 스테이트 업데이트 호출
		/// </summary>
		void Update(float dt);

		/// <summary>
		/// 애니메이터를 반환합니다
		/// </summary>
		Animator* GetAnimator()const { return mAnimator; }

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
		void UpdateAnimation(float dt);

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
		StateName GetCurrentStateName() const { return mCurrentState->first; }

		/// <summary>
		/// 현재 애니메이션의 애니메이션 인터페이스를 반환합니다 
		/// </summary>
		fq::graphics::IAnimation& GetCurrentStateAnimation() const { return mCurrentState->second.GetAnimation(); }
		std::shared_ptr<fq::graphics::IAnimation> GetSharedCurrentStateAnimation() const { return mCurrentState->second.GetSharedAnimation(); }
		const std::shared_ptr<fq::graphics::IAnimation>& GetSharedRefCurrentStateAnimation() const { return mCurrentState->second.GetSharedRefAnimation(); }
		bool GetHasCurrentStateAnimation() const { return mCurrentState->second.GetHasAnimation(); }

		/// <summary>
		/// 다음 애니메이션 이름을 반환합니다  
		/// </summary>
		StateName GetNextStateName()const;

		/// <summary>
		/// 다음 애니메이션의 애니메이션 인터페이스를 반환합니다
		/// </summary>
		fq::graphics::IAnimation& GetNextStateAnimation() const;
		std::shared_ptr<fq::graphics::IAnimation> GetSharedNextStateAnimationOrNull() const;
		const std::shared_ptr<fq::graphics::IAnimation>& GetSharedRefNextStateAnimation() const;
		bool GetHasNextStateAnimation() const;

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
		void AddTransition(AnimationTransition transition);

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

		TransitionMap& GetTransitionMap() { return mTransitions; }
		const TransitionMap& GetTransitionMap() const { return mTransitions; }

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

		/// <summary>
		/// 블렌드 웨이트를 반환합니다.
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