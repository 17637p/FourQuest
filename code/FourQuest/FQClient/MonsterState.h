#pragma once
#include "MonsterBaseState.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
#pragma region Idle
	class MonsterIdleState : public MonsterBaseState
	{
	public:
		MonsterIdleState(std::shared_ptr<Monster> monster);
		virtual ~MonsterIdleState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
	};
#pragma endregion Idle

#pragma region Find
	class MonsterFindState : public MonsterBaseState
	{
	public:
		MonsterFindState(std::shared_ptr<Monster> monster);
		virtual ~MonsterFindState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
	};
#pragma endregion Find

#pragma region Chase
	class MonsterChaseState : public MonsterBaseState
	{
	public:
		MonsterChaseState(std::shared_ptr<Monster> monster);
		virtual ~MonsterChaseState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
		void MoveToTarget(float dist);
		void rotateTowards();
		DirectX::SimpleMath::Vector3 getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion);

	private:
		float mMoveDistance;
	};
#pragma endregion Chase

#pragma region Damaged
	class MonsterDamagedState : public MonsterBaseState
	{
	public:
		MonsterDamagedState(std::shared_ptr<Monster> monster);
		virtual ~MonsterDamagedState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
	};
#pragma endregion Damaged

#pragma region Death
	class MonsterDeathState : public MonsterBaseState
	{
	public:
		MonsterDeathState(std::shared_ptr<Monster> monster);
		virtual ~MonsterDeathState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
	};
#pragma endregion Death

#pragma region AttatchPreparation
	class MonsterAttatchPreparationState : public MonsterBaseState
	{
	public:
		MonsterAttatchPreparationState(std::shared_ptr<Monster> monster);
		virtual ~MonsterAttatchPreparationState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
		DirectX::SimpleMath::Vector3 getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion);
		void rotateTowards();
		void makeAttackRangeCollider();
	};
#pragma endregion AttatchPreparation

#pragma region AttatchIdle
	class MonsterAttatchIdleState : public MonsterBaseState
	{
	public:
		MonsterAttatchIdleState(std::shared_ptr<Monster> monster);
		virtual ~MonsterAttatchIdleState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
		float mWaitTime;
	};
#pragma endregion AttatchIdle

#pragma region AttatchExecution
	class MonsterAttatchExecutionState : public MonsterBaseState
	{
	public:
		MonsterAttatchExecutionState(std::shared_ptr<Monster> monster);
		virtual ~MonsterAttatchExecutionState();

		virtual void OnStateEnter() override;
		virtual void OnStateUpdate(float deltaTime) override;
		virtual void OnStateExit() override;

	private:
	};
#pragma endregion AttatchExecution
}

