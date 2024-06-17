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

	if (cloneHpBar == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneHpBar = game_module::ObjectPool::GetInstance()->Assign<HpBarUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneHpBar = *this;
	}

	return cloneHpBar;
}

void fq::client::HpBarUI::OnUpdate(float dt)
{

}
