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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 움직이는 방향에 따라서 회전 방향의 동기화 여부를 반환합니다
		/// </summary>
		bool IsSyncRotationWithMovementDirection() const { return mbSyncRotationWithMovementDirection; }
		
		/// <summary>
		/// 움직이는 방향에 따라서 회전 방향의 동기화 여부를 설정합니다
		/// </summary>	
		void SetSyncRotationWithMovementDirection(bool val) { mbSyncRotationWithMovementDirection = val; }
		
		/// <summary>
		/// 현재 Agent가 정지상태인지 반환합니다.
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