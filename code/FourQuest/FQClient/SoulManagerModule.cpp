#include "SoulManagerModule.h"

#include "Soul.h"
#include "Player.h"
#include "SoulVariable.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	SoulManagerModule::SoulManagerModule()
	{
		mPlayerType.resize(4, EPlayerType::NONE);
		mPlayerArmourDeathCount.resize(4, 0);
	}
	SoulManagerModule::~SoulManagerModule()
	{
	}

	void SoulManagerModule::Set(fq::game_module::Scene* scene)
	{
		mScene = scene;
	}

	void SoulManagerModule::OnStart()
	{
		// ���� ��ȯ�Ͽ� ���� �ִ� ������Ʈ�� �����ϴ� �÷��̾� ��Ʈ�ѷ��� �ִ� �� Ȯ�� �� �Ŀ� �迭�� ������ ����
		for (auto& object : mScene->GetComponentView<fq::game_module::CharacterController>())
		{
			unsigned int id = object.GetComponent<fq::game_module::CharacterController>()->GetControllerID();

			if (id >= 4)
			{
				spdlog::error("ERROR : ID is greater than 4!");
				return;
			}
			
			mPlayerArmourDeathCount[id] = 0;

			if (object.HasComponent<Player>())
				mPlayerType[id] = EPlayerType::LivingArmour;
			else if (object.HasComponent<Soul>())
				mPlayerType[id] = EPlayerType::Soul;
		}
	}

	void SoulManagerModule::OnUpdate(float dt)
	{
		checkSummonSoul(dt);
		checkGameOver();
	}

	void SoulManagerModule::checkGameOver()
	{
		// ���� �������� üũ
		bool isGameOver = true;
		for (auto& isDead : mPlayerType)
		{
			if (isDead != EPlayerType::SoulDestoryed)
				isGameOver = false;
		}

		if (isGameOver)
		{
		}
	}

	void SoulManagerModule::checkSummonSoul(float dt)
	{
		// ���� �ش� ��ȥ�� ��ȯ�� �Ǿ����� ����
		mSoulSummonQueue.erase(std::remove_if(mSoulSummonQueue.begin(), mSoulSummonQueue.end()
			, [](const SoulData& data)
			{
				return data.durationTime >= SoulVariable::OutTime;
			}), mSoulSummonQueue.end());

		// ��ȥ�� ��ȯ �ð��� �Ǿ����� ���� ��ȥ ����
		for (auto& data : mSoulSummonQueue)
		{
			data.durationTime += dt;

			if (data.durationTime >= SoulVariable::OutTime)
			{
				auto instance = mScene->GetPrefabManager()->InstantiatePrefabResoure(data.soulPrefab);
				auto& soul = *(instance.begin());

				// ��Ʈ�ѷ� ����
				soul->GetComponent<game_module::CharacterController>()
					->SetControllerID(data.id);

				// �ҿ� Ÿ�� ����
				soul->GetComponent<Soul>()->SetSoulType(data.soulType);

				// ��ġ ����
				soul->GetComponent<game_module::Transform>()->SetWorldMatrix(data.worldTransform);

				mScene->AddGameObject(soul);
			}
		}
	}

	void SoulManagerModule::SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab)
	{
		SoulData data;

		data.id = id;
		data.soulType = soulType;
		data.worldTransform = worldTransform;
		data.soulPrefab = soulPrefab;

		mSoulSummonQueue.push_back(data);
	}

	ESoulType SoulManagerModule::GetDestroyArmourSoulType(unsigned int id)
	{
		for (auto& data : mSoulSummonQueue)
		{
			if (data.id == id)
				return data.soulType;
		}

		return ESoulType::End;
	}

	int SoulManagerModule::GetPlayerDeathCount(unsigned int id)
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

	EPlayerType SoulManagerModule::GetPlayerType(unsigned int id)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!");
			return EPlayerType::NONE;
		}
		else
		{
			return mPlayerType[id];
		}
	}

	void SoulManagerModule::AddPlayerArmourDeathCount(unsigned int id)
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

	void SoulManagerModule::SetPlayerType(unsigned int id, const EPlayerType& playerType)
	{
		if (id >= 4)
		{
			spdlog::error("ERROR : ID is greater than 4!");
			return;
		}
		else
		{
			mPlayerType[id] = playerType;
		}
	}
}
