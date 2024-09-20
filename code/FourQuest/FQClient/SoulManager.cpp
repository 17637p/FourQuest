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
		// 씬을 순환하여 씬에 있는 오브젝트를 조종하는 플레이어 컨트롤러가 있는 지 확인 한 후에 배열에 데이터 저장
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

			// 플레이어가 다 죽었기 때문에 메뉴창으로 돌아갈 것인지 죽음 엔딩 창을 띄울 것인지 코드를 작성하면 됩니다.

		}
	}

	std::shared_ptr<fq::game_module::Component> SoulManager::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneSoulManager = std::dynamic_pointer_cast<SoulManager>(clone);

		if (cloneSoulManager == nullptr) // 새로 생성해서 복사본을 준다소울라이크
		{
			cloneSoulManager = game_module::ObjectPool::GetInstance()->Assign<SoulManager>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneSoulManager = *this;
		}

		return cloneSoulManager;
	}
}
