#include "PlayerUI.h"

fq::client::PlayerUI::PlayerUI()
{

}

fq::client::PlayerUI::~PlayerUI()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::PlayerUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePlayerUI = std::dynamic_pointer_cast<PlayerUI>(clone);

	if (clonePlayerUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		clonePlayerUI = game_module::ObjectPool::GetInstance()->Assign<PlayerUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*clonePlayerUI = *this;
	}

	return clonePlayerUI;
}
