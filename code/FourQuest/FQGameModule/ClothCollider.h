#pragma once
#include "Component.h"


namespace fq::game_module
{
	using ClothPath = std::string;

	class ClothCollider : public Component
	{
	public:
		ClothCollider();
		~ClothCollider();


	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override;


	private:
		ClothPath mClothPath;

		float mClothMass;
		float mRestOffset;
	};
}

