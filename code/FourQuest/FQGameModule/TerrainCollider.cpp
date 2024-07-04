#include "TerrainCollider.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::TerrainCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCollider = std::dynamic_pointer_cast<TerrainCollider>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = ObjectPool::GetInstance()->Assign<TerrainCollider>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}


	return cloneCollider;
}

fq::game_module::TerrainCollider::TerrainCollider()
{

}

fq::game_module::TerrainCollider::~TerrainCollider()
{

}
