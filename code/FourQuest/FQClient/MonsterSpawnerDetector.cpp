#include "MonsterSpawnerDetector.h"

#include "../FQGameModule/GameModule.h"
#include "MonsterSpawner.h"

std::shared_ptr<fq::game_module::Component> fq::client::MonsterSpawnerDetector::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MonsterSpawnerDetector> cloneMonster = std::dynamic_pointer_cast<MonsterSpawnerDetector>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawnerDetector>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MonsterSpawnerDetector::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player
		|| collision.other->GetTag() == game_module::ETag::Dash
		|| collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
	{
		auto parent = GetGameObject()->GetParent();
		if (parent)
		{
			auto monsterSpawner = parent->GetComponent<MonsterSpawner>();
			monsterSpawner->DetectPlayer(collision.other);
			// Ȱ��ȭ ���Ŀ��� �ı��մϴ�
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}
