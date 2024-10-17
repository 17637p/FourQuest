#include "ArmourDestroyer.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourDestroyer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloned = std::dynamic_pointer_cast<ArmourDestroyer>(clone);

	if (cloned == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<ArmourDestroyer>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloned = *this;
	}

	return cloned;
}

void fq::client::ArmourDestroyer::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		auto player = collision.other->GetComponent<Player>();

		if (mbIsRemainDeadAromour)
		{
			player->RemainDeadArmour();
		}

		if (player)
		{
			player->SummonSoul(false);
		}
	}
}
