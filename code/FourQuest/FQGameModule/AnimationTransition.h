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
		std::string GetNextState() const { return mNextState; }
		
		/// <summary>
		/// ���� ������Ʈ ����� �̸��� �����մϴ� 
		/// </summary>
		void SetNextState(std::string state) { mNextState = state; }

	private:
		std::string mNextState; 
		std::vector<TransitionCondition> mConditions;
	};


}