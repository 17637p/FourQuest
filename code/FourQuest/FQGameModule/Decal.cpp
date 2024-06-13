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

	if (cloneDecal == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneDecal = ObjectPool::GetInstance()->Assign<Decal>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneDecal = *this;
	}

	cloneDecal->mDecalObjectInterface = nullptr;

	return cloneDecal;
}
