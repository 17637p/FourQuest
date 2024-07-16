#include "Articulation.h"

#include "ArticulationData.h"
#include "ArticulationLoader.h"

namespace fq::game_module
{
	Articulation::Articulation()
		: mID()
		, mCollisionCount()
		, mbIsRagdoll(true)
		, mArticulationData()
		, mArticulationPath()
		, mOffset{}
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

	void Articulation::Load()
	{
		fq::game_module::ArticulationLoader loader;

		mArticulationData = loader.LoadArticulationData(mArticulationPath);
	}

	entt::meta_handle Articulation::GetHandle()
	{
		return *this;
	}

}
