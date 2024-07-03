#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	/// <summary>
	/// �ͷ��� �ݶ��̴� 
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