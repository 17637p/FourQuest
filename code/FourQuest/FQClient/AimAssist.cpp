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

	if (cloneAssist == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<AimAssist>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAssist = *this;
	}

	return cloneAssist;
}
