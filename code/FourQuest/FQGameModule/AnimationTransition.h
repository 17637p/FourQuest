#pragma once

#include <string>
#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	/// <summary>
	/// �ִϸ��̼� ���� ���� 
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
		/// ���� ������ üũ�մϴ� 
		/// </summary>
		bool CheckCondition(Parameter parameter);

		/// <summary>
		/// ���� �Ķ������ ���̵� ��ȯ�մϴ� 
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
		/// ������ �߰��մϴ� 
		/// </summary>
		void PushBackCondition(TransitionCondition::CheckType type
			, TransitionCondition::ParameterID id
			, TransitionCondition::Parameter compareParam);
		
		/// <summary>
		/// ������ ������ �����մϴ� 
		/// </summary>
		void PopBackCondition();

		/// <summary>
		/// ���ǵ��� ���� �����̳ʸ� ��ȯ�մϴ�
		/// </summary>
		const std::vector<TransitionCondition>& GetConditions()const { return mConditions; };

		/// <summary>
		/// ���� ������Ʈ ����� �̸��� ��ȯ�մϴ� 
		/// </summary>
		std::string GetEnterState() const { return mEnterState; }
		
		/// <summary>
		/// ���� ������Ʈ ����� �̸��� �����մϴ� 
		/// </summary>
		void SetEnterState(std::string state) { mEnterState = state; }

		std::string GetExitState() const { return mExitState; }
		void SetExitState(std::string val) { mExitState = val; }
	private:
		std::string mExitState;
		std::string mEnterState; 
		std::vector<TransitionCondition> mConditions;
	};


}