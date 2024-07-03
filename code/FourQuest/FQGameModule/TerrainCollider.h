#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	/// <summary>
	/// 터레인 콜라이더 
	/// </summary>
	class TerrainCollider : public Component
	{
	public:
		TerrainCollider();
		~TerrainCollider();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;


	private:

	};

}