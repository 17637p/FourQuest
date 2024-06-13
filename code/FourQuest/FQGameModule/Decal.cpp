#define NOMINMAX
#include "Decal.h"

#include <algorithm>

fq::game_module::Decal::Decal()
	: mDecalObjectInterface(nullptr)
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::Decal::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneDecal = std::dynamic_pointer_cast<Decal>(clone);

	if (cloneDecal == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneDecal = ObjectPool::GetInstance()->Assign<Decal>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneDecal = *this;
	}

	cloneDecal->mDecalObjectInterface = nullptr;

	return cloneDecal;
}
