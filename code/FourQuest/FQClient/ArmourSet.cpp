#include "ArmourSet.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourSet::Clone(std::shared_ptr<game_module::Component> clone /* = nullptr */) const
{
	auto cloneArmourSet = std::dynamic_pointer_cast<ArmourSet>(clone);

	if (cloneArmourSet == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmourSet = game_module::ObjectPool::GetInstance()->Assign<ArmourSet>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneArmourSet = *this;
	}

	return cloneArmourSet;
}

fq::client::ArmourSet::ArmourSet()
	:mArmourList()
{
}

fq::client::ArmourSet::~ArmourSet()
{
}

