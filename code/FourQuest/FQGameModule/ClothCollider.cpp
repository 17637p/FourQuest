#include "ClothCollider.h"

namespace fq::game_module
{
	ClothCollider::ClothCollider()
	{
	}
	ClothCollider::~ClothCollider()
	{
	}
	entt::meta_handle fq::game_module::ClothCollider::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<fq::game_module::Component> fq::game_module::ClothCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		auto cloneCollider = std::dynamic_pointer_cast<ClothCollider>(clone);

		if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneCollider = ObjectPool::GetInstance()->Assign<ClothCollider>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneCollider = *this;
		}

		cloneCollider->mClothInfo.id = fq::physics::unregisterID;

		return cloneCollider;
	}
}
