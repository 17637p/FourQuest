#pragma once

#include <string>
#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	/// <summary>
	/// 애니메이션 전이 조건 
	/// </summary>
	class TransitionCondition
	{
	public:
		using ParameterID = std::string;
		using Parameter = entt::meta_any;

		enum class CheckType
		{
			Greater,
			Less,
			Equals,
			NotEqual,
		};

		TransitionCondition(CheckType type
			, ParameterID compareID
			, Parameter comparCondition);

		/// <summary>
		/// 전이 조건을 체크합니다 
		/// </summary>
		bool CheckCondition(Parameter parameter);

		/// <summary>
		/// 비교할 파라미터의 아이디를 반환합니다 
		/// </summary>
		ParameterID GetParameterID()const { return mParameterID; }

	private:
		CheckType mCheckType;
		ParameterID mParameterID;
		Parameter mCompareParameter;
	};

	class AnimationTransition
	{
	public:
		AnimationTransition();
		AnimationTransition(std::string prev, std::string next);

		/// <summary>
		/// 조건을 추가합니다 
		/// </summary>
		void PushBackCondition(TransitionCondition::CheckType type
			, TransitionCondition::ParameterID id
			, TransitionCondition::Parameter compareParam);
		
		/// <summary>
		/// 마지막 조건을 제거합니다 
		/// </summary>
		void PopBackCondition();

		/// <summary>
		/// 조건들을 담은 컨테이너를 반환합니다
		/// </summary>
		const std::vector<TransitionCondition>& GetConditions()const { return mConditions; };

		/// <summary>
		/// 다음 스테이트 노드의 이름을 반환합니다 
		/// </summary>
		std::string GetNextState() const { return mNextState; }
		
		/// <summary>
		/// 다음 스테이트 노드의 이름을 설정합니다 
		/// </summary>
		void SetNextState(std::string state) { mNextState = state; }

		std::string GetPrevState() const { return mPrevState; }
		void SetPrevState(std::string val) { mPrevState = val; }
	private:
		std::string mPrevState;
		std::string mNextState; 
		std::vector<TransitionCondition> mConditions;
	};


}