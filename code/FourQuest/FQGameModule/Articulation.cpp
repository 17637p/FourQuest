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

		if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneCollider = ObjectPool::GetInstance()->Assign<Articulation>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
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
