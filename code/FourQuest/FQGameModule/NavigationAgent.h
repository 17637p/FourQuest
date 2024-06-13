#pragma once
#include "Component.h"

namespace fq::game_module
{
	class NavigationField;
	class NavigationAgent : public Component
	{
	public:
		class Impl;

		NavigationAgent();
		virtual ~NavigationAgent();

		virtual void OnUpdate(float dt) override;

		void AssignToNavigationField(NavigationField* navField);
		
		float GetSpeed();
		void SetSpeed(float speed);

		float GetAcceleration();
		void SetAcceleration(float accel);

		float GetRadius();
		void SetRadius(float radius);

		DirectX::SimpleMath::Vector3 GetTargetPosition();
		void MoveTo(DirectX::SimpleMath::Vector3 destination);

	private:
		Impl* impl;
		NavigationField* navField;
		friend NavigationField;
	};
}