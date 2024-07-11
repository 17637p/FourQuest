#include "Articulation.h"

#include "ArticulationData.h"

namespace fq::game_module
{
	Articulation::Articulation()
	{
	}

	Articulation::~Articulation()
	{
	}

	std::shared_ptr<Component> Articulation::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneCollider = std::dynamic_pointer_cast<Articulation>(clone);

		if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneCollider = ObjectPool::GetInstance()->Assign<Articulation>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneCollider = *this;
		}

		cloneCollider->mCollisionCount = 0;

		return cloneCollider;
	}

	entt::meta_handle Articulation::GetHandle()
	{
		return *this;
	}

}
