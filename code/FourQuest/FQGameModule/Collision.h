#pragma once

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class GameObject;
	class Component;
}

namespace fq::game_module
{
	// �浹 ���� 
	struct Collision
	{
		GameObject* object;
		GameObject* other;

		const std::vector<DirectX::SimpleMath::Vector3>& contactPoints;
	};
}