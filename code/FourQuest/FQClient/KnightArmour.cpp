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

	if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<KnightArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
