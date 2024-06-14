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

		void RegisterNavigationField(fq::game_engine::PathFindingSystem* navField);
		
		float GetSpeed();
		void SetSpeed(float speed);

		float GetAcceleration();
		void SetAcceleration(float accel);

		float GetRadius();
		void SetRadius(float radius);

		int GetAgentIndex();

		DirectX::SimpleMath::Vector3 GetTargetPosition();
		void MoveTo(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		Impl* impl;
		fq::game_engine::PathFindingSystem* mPathFindingSystem;
	};
}