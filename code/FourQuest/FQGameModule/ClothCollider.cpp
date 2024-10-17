#include "ClothCollider.h"

#include "ClothDataLoader.h"

namespace fq::game_module
{
	ClothCollider::ClothCollider()
		: mClothPath{}
	{
	}

	ClothCollider::~ClothCollider()
	{
	}

	void ClothCollider::Load()
	{
		ClothDataLoader loader;

		auto clothData = loader.LoadArticulationData(mClothPath);

		mClothInfo->vertices = clothData->vertices.data();
		mClothInfo->vertexSize = clothData->vertices.size();
		mClothInfo->indices = clothData->indices.data();
		mClothInfo->indexSize = clothData->indices.size();
		mClothInfo->disableIndices = clothData->disableIndices.data();
		mClothInfo->disableIndexSize = clothData->disableIndices.size();
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

		cloneCollider->mClothInfo->id = fq::physics::unregisterID;

		return cloneCollider;
	}
}
