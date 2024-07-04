#include "TerrainCollider.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::TerrainCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCollider = std::dynamic_pointer_cast<TerrainCollider>(clone);

	if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCollider = ObjectPool::GetInstance()->Assign<TerrainCollider>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
