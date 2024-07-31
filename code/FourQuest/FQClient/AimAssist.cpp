#include "AimAssist.h"


void fq::client::AimAssist::OnTriggerEnter(const game_module::Collision& collision)
{

}

void fq::client::AimAssist::OnTriggerExit(const game_module::Collision& collision)
{

}

fq::client::AimAssist::~AimAssist()
{

}

fq::client::AimAssist::AimAssist()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::AimAssist::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<AimAssist>(clone);

	if (cloneAssist == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<AimAssist>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAssist = *this;
	}

	return cloneAssist;
}
