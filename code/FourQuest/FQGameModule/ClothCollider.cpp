#include "ClothCollider.h"

#include "ClothDataLoader.h"

namespace fq::game_module
{
	ClothCollider::ClothCollider()
		: mClothPath{}
		, mClothInfo(std::make_shared<ClothInfo>())
		, mbIsSkinnedMesh(false)
	{
	}

	ClothCollider::~ClothCollider()
	{
	}

	bool ClothCollider::Load()
	{
		ClothDataLoader loader;
		return loader.LoadArticulationData(mClothPath, mClothInfo->clothData);
	}

	entt::meta_handle fq::game_module::ClothCollider::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<fq::game_module::Component> fq::game_module::ClothCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		auto cloneCollider = std::dynamic_pointer_cast<ClothCollider>(clone);

		if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneCollider = ObjectPool::GetInstance()->Assign<ClothCollider>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneCollider = *this;
		}

		cloneCollider->mClothInfo->id = fq::physics::unregisterID;

		return cloneCollider;
	}
}
