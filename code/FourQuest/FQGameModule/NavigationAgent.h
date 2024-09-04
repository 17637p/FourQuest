#pragma once
#include "Component.h"

namespace fq::game_engine { class PathFindingSystem; }

namespace fq::game_module
{
	class NavigationAgent : public Component
	{
	public:
		class Impl;

		NavigationAgent();
		virtual ~NavigationAgent();

		virtual void OnUpdate(float dt) override;

		void CreateAgentData();
		void DeleteAgentData();
		void RegisterNavigationField(fq::game_engine::PathFindingSystem* pathFindingSystem);
		
		float GetSpeed()const;
		void SetSpeed(float speed);

		float GetAcceleration();
		void SetAcceleration(float accel);

		float GetRadius();
		void SetRadius(float radius);

		void SetAgentState();

		int GetAgentIndex();

		DirectX::SimpleMath::Vector3 GetTargetPosition();
		void MoveTo(DirectX::SimpleMath::Vector3 destination);
		void Stop();

		bool HasReachedDestination()const;

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// �����̴� ���⿡ ���� ȸ�� ������ ����ȭ ���θ� ��ȯ�մϴ�
		/// </summary>
		bool IsSyncRotationWithMovementDirection() const { return mbSyncRotationWithMovementDirection; }
		
		/// <summary>
		/// �����̴� ���⿡ ���� ȸ�� ������ ����ȭ ���θ� �����մϴ�
		/// </summary>	
		void SetSyncRotationWithMovementDirection(bool val) { mbSyncRotationWithMovementDirection = val; }
		
		/// <summary>
		/// ���� Agent�� ������������ ��ȯ�մϴ�.
		/// </summary>
		bool IsStopAgent() const { return mbIsStop; }
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		bool mbSyncRotationWithMovementDirection;
		bool mbIsStop;

		Impl* mImpl;
		fq::game_engine::PathFindingSystem* mPathFindingSystem;
	};
}