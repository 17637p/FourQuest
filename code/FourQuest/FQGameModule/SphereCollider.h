#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class SphereCollider : public Component
	{
		using SphereColliderInfo = fq::physics::SphereColliderInfo;

	public:
		SphereCollider();
		~SphereCollider();

		Component* Clone(Component* clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override;

	private:
		SphereColliderInfo mSphereColliderInfo;
	};


}