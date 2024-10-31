#include "FillSoulGauge.h"
#include "Player.h"

fq::client::FillSoulGauge::FillSoulGauge()
	: mFillGauge(500.f)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::FillSoulGauge::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<FillSoulGauge> cloneMonster = std::dynamic_pointer_cast<FillSoulGauge>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<FillSoulGauge>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::FillSoulGauge::OnTriggerEnter(const game_module::Collision& collision)
{
	auto player = collision.other->GetComponent<Player>();

	if (player == nullptr)
	{
		return;
	}

	player->SetSoulGauge(mFillGauge);
}
