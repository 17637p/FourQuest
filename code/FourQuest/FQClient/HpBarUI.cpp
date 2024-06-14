#include "HpBarUI.h"

fq::client::HpBarUI::~HpBarUI()
{

}

fq::client::HpBarUI::HpBarUI()
	:mIsVisible(false)
{}

std::shared_ptr<fq::game_module::Component> fq::client::HpBarUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneHpBar = std::dynamic_pointer_cast<HpBarUI>(clone);

	if (cloneHpBar == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneHpBar = game_module::ObjectPool::GetInstance()->Assign<HpBarUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneHpBar = *this;
	}

	return cloneHpBar;
}

void fq::client::HpBarUI::OnUpdate(float dt)
{

}
