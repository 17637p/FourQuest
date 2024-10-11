#include "ClothCollider.h"

namespace fq::game_module
{
	ClothCollider::ClothCollider()
	{
	}
	ClothCollider::~ClothCollider()
	{
	}
	std::shared_ptr<Component> ClothCollider::Clone(std::shared_ptr<Component> clone) const
	{
		return std::shared_ptr<Component>();
	}
	entt::meta_handle ClothCollider::GetHandle()
	{
		return entt::meta_handle();
	}
}
