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
		bool CheckCondition(Parameter parameter)const;

		/// <summary>
		/// ���� �Ķ������ ���̵� ��ȯ�մϴ� 
		/// </summary>
		ParameterID GetParameterID()const { return mParameterID; }

		/// <summary>
		/// ���� �Ķ���� ���̵� �����մϴ� 
		/// </summary>
		void SetParameterID(ParameterID id) { mParameterID = id; }

		/// <summary>
		/// ���� �����ڸ� ��ȯ�մϴ� 
		/// </summary>
		CheckType GetCheckType() const { return mCheckType; }

		/// <summary>
		/// ���� �����ڸ� �����մϴ� 
		/// </summary>
		void SetCheckType(CheckType val) { mCheckType = val; }

		/// <summary>
		/// ���� �Ķ���͸� ��ȯ�մϴ� 
		/// </summary>
		Parameter GetCompareParameter() const { return mCompareParameter; }

		/// <summary>
		/// ���� �Ķ���͸� �����մϴ� 
		/// </summary>
		void SetCompareParameter(Parameter val);

	private:
		CheckType mCheckType;
		ParameterID mParameterID;
		Parameter mCompareParameter;
	};

	class AnimationTransition
	{
	public:
		enum class InterruptionSource
		{
			None,
			CurrentState,
			NextState,
			CurrentStateThenNextState,
			NextStateThenCurrentState
		};

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
		std::vector<TransitionCondition>& GetConditions() { return mConditions; };

		/// <summary>
		/// ���ǵ��� ���� �����̳ʸ� ��ȯ�մϴ�
		/// </summary>
		const std::vector<TransitionCondition>& GetConditions() const { return mConditions; };

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

		/// <summary>
		/// �ִϸ��̼� ��ȯ �ð��� ��ȯ�մϴ�.
		/// </summary>
		float GetTransitionDuration() const { return mTransitionDuration; }

		/// <summary>
		/// �ִϸ��̼� ��ȯ �ð��� ��ȯ�մϴ�.
		/// </summary>
		void SetTransitionDuration(float val) { mTransitionDuration = val; }

		/// <summary>
		/// ��ȯ���¿��� �ٸ� ��ȯ��û�� ���� ������ ��ȯ�մϴ�.
		/// </summary>
		InterruptionSource GetInterruptionSource() const { return mInterruptionSource; }

		/// <summary>
		/// ��ȯ���¿��� �ٸ� ��ȯ��û�� ���� ������ �����մϴ�.
		/// </summary>
		void SetInterruptionSource(InterruptionSource val) { mInterruptionSource = val; }

		/// <summary>
		/// Ż�� �ð��� �ִ��� Ȯ���մϴ�.
		/// </summary>
		bool HasExitTime()const;

		/// <summary>
		/// �ִϸ��̼� Ż�� �ð��� ��ȯ�մϴ�. 
		/// </summary>
		float GetExitTime() const { return mExitTime; }

		/// <summary>
		/// �ִϸ��̼� Ż��ð��� �����մϴ� 
		/// </summary>
		void SetExitTime(float second) { mExitTime = second; }

		/// <summary>
		/// ���� ���·� �ٽ� ��ȯ�� �������� ��ȯ�մϴ�
		/// </summary>
		bool CanTrasitionToSelf() const { return mbCanTrasitionToSelf; }
		
		/// <summary>
		/// ���� ���·� �ٽ� ��ȯ�� �������� �����մϴ�
		/// </summary>
		void SetCanTrasitionToSelf(bool canTransitionToSelf) { mbCanTrasitionToSelf = canTransitionToSelf; }

	public:
		static constexpr float NoExitTime = -1.f;

	private:
		std::string mExitState;
		std::string mEnterState;

		float mExitTime = NoExitTime; 
		float mTransitionDuration;
		InterruptionSource mInterruptionSource;
		bool mbCanTrasitionToSelf;

		std::vector<TransitionCondition> mConditions;
	};
}