#include "BossHP.h"

std::shared_ptr<fq::game_module::Component> fq::client::BossHP::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneBossHP = std::dynamic_pointer_cast<BossHP>(clone);

	if (cloneBossHP == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneBossHP = game_module::ObjectPool::GetInstance()->Assign<BossHP>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
