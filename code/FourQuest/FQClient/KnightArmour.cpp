#include "KnightArmour.h"

fq::client::KnightArmour::KnightArmour()
{

}

fq::client::KnightArmour::~KnightArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::KnightArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<KnightArmour>(clone);

	if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<KnightArmour>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneArmour = *this;
	}


	return cloneArmour;
}

void fq::client::KnightArmour::EmitSwordAttack()
{

}

void fq::client::KnightArmour::EmitShieldAttack()
{

}

void fq::client::KnightArmour::EmitDashAttack()
{

}
