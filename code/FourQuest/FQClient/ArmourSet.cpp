#include "ArmourSet.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourSet::Clone(std::shared_ptr<game_module::Component> clone /* = nullptr */) const
{
	auto cloneArmourSet = std::dynamic_pointer_cast<ArmourSet>(clone);

	if (cloneArmourSet == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneArmourSet = game_module::ObjectPool::GetInstance()->Assign<ArmourSet>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

