#include "ArmourDestroyer.h"

#include "../FQGameModule/GameModule.h"
#include "Player.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourDestroyer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloned = std::dynamic_pointer_cast<ArmourDestroyer>(clone);

	if (cloned == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<ArmourDestroyer>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
