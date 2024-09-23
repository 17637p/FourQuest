#include "DebuffPoisonZone.h"

#include "Player.h"

namespace fq::client
{
	DebuffPoisonZone::DebuffPoisonZone()
		: mPoisonDamage()
		, mPoisonTurm()
		, mDurationTime()
	{
	}

	DebuffPoisonZone::~DebuffPoisonZone()
	{
	}

	std::shared_ptr<fq::game_module::Component> DebuffPoisonZone::Clone(std::shared_ptr<Component> clone) const
	{
		std::shared_ptr<DebuffPoisonZone> cloneAttack = std::dynamic_pointer_cast<DebuffPoisonZone>(clone);

		if (cloneAttack == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<DebuffPoisonZone>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
