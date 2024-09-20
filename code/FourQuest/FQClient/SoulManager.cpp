#include "SoulManager.h"

#include "Soul.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

namespace fq::client
{
	SoulManager::SoulManager()
		: mPlayerArmourDeathCount{}
		, mbIsPlayerSoulDeath{}
	{
	}
	SoulManager::~SoulManager()
	{
	}

	int SoulManager::GetPlayerDeathCount(unsigned int id)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!");
			return -1;
		}
		else
		{
			return mPlayerArmourDeathCount[id];
		}
	}

	int SoulManager::GetbIsPlayerDeath(unsigned int id)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!"); 
			return -1;
		}
		else
		{
			return mbIsPlayerSoulDeath[id];
		}
	}

	void SoulManager::AddPlayerArmourDeathCount(unsigned int id)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!");
			return;
		}
		else
		{
			mPlayerArmourDeathCount[id]++;
		}
	}

	void SoulManager::SetbIsPlayerSoulDeath(unsigned int id, bool isDeath)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!");
			return;
		}
		else
		{
			mbIsPlayerSoulDeath[id] = isDeath;
		}
	}

	void SoulManager::OnStart()
	{
		// ���� ��ȯ�Ͽ� ���� �ִ� ������Ʈ�� �����ϴ� �÷��̾� ��Ʈ�ѷ��� �ִ� �� Ȯ�� �� �Ŀ� �迭�� ������ ����
		for (auto& object : GetScene()->GetComponentView<fq::game_module::CharacterController>())
		{
			unsigned int id = object.GetComponent<fq::game_module::CharacterController>()->GetControllerID();

			if (id >= 4)
			{
				spdlog::error("ERROR : ID is greater than 4!");
				return;
			}
			
			mPlayerArmourDeathCount[id] = 0;
			mbIsPlayerSoulDeath[id] = false;
		}
	}

	void SoulManager::OnFixedUpdate(float dt)
	{
		for (bool isDead : mbIsPlayerSoulDeath)
		{
			if (!isDead)
				return;

			// �÷��̾ �� �׾��� ������ �޴�â���� ���ư� ������ ���� ���� â�� ��� ������ �ڵ带 �ۼ��ϸ� �˴ϴ�.

		}
	}

	std::shared_ptr<fq::game_module::Component> SoulManager::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneSoulManager = std::dynamic_pointer_cast<SoulManager>(clone);

		if (cloneSoulManager == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
		{
			cloneSoulManager = game_module::ObjectPool::GetInstance()->Assign<SoulManager>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneSoulManager = *this;
		}

		return cloneSoulManager;
	}
}
