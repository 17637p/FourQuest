#include "BossHP.h"

std::shared_ptr<fq::game_module::Component> fq::client::BossHP::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneBossHP = std::dynamic_pointer_cast<BossHP>(clone);

	if (cloneBossHP == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneBossHP = game_module::ObjectPool::GetInstance()->Assign<BossHP>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneBossHP = *this;
	}

	return cloneBossHP;
}

void fq::client::BossHP::OnStart()
{

	//mHPBarGauge = ->GetComponent<fq::game_module::ImageUI>();
}

void fq::client::BossHP::OnUpdate(float dt)
{

}
