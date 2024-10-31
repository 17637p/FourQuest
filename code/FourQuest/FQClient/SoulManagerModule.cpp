#include "SoulManagerModule.h"

#include "Soul.h"
#include "Player.h"
#include "SoulVariable.h"
#include "CameraMoving.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	SoulManagerModule::SoulManagerModule()
		: mScene(nullptr)
		, mSoulSummonQueue{}
	{
	}
	SoulManagerModule::~SoulManagerModule()
	{
		mSoulSummonQueue.clear();
	}

	void SoulManagerModule::OnStart(fq::game_module::Scene* scene)
	{
		SoulVariable::Player1DeathCount = 0;
		SoulVariable::Player2DeathCount = 0;
		SoulVariable::Player3DeathCount = 0;
		SoulVariable::Player4DeathCount = 0;

		mScene = scene;
	}

	void SoulManagerModule::OnUpdate(float dt)
	{
		checkSummonSoul(dt);
	}

	bool SoulManagerModule::CheckGameOver()
	{
		// ���� �������� üũ
		if ((SoulVariable::Player1Type == EPlayerType::SoulDestoryed || SoulVariable::Player1Type == EPlayerType::None)
			&& (SoulVariable::Player2Type == EPlayerType::SoulDestoryed || SoulVariable::Player2Type == EPlayerType::None)
			&& (SoulVariable::Player3Type == EPlayerType::SoulDestoryed || SoulVariable::Player3Type == EPlayerType::None)
			&& (SoulVariable::Player4Type == EPlayerType::SoulDestoryed || SoulVariable::Player4Type == EPlayerType::None))
		{
			return true;
		}

		return false;
	}

	void SoulManagerModule::checkSummonSoul(float dt)
	{
		// ��ȥ�� ��ȯ �ð��� �Ǿ����� ���� ��ȥ ����
		for (auto& data : mSoulSummonQueue)
		{
			data.durationTime += dt;

			if (data.durationTime >= SoulVariable::OutTime)
			{
				// �ӽ� ������ ����
				mScene->ViewComponents<CameraMoving>([data](game_module::GameObject& object, CameraMoving& camera)
					{
						camera.DeletePlayerTransform(data.tempGameObject->GetComponent<game_module::Transform>());
					});

				auto instance = mScene->GetPrefabManager()->InstantiatePrefabResoure(data.soulPrefab);
				auto& soul = *(instance.begin());

				// ��Ʈ�ѷ� ����
				soul->GetComponent<game_module::CharacterController>()
					->SetControllerID(data.id);

				// �ҿ� Ÿ�� ����
				soul->GetComponent<Soul>()->SetSoulType(data.soulType);

				// ��ġ ����
				soul->GetComponent<game_module::Transform>()->SetWorldMatrix(data.worldTransform);

				// �ҿ� ������ ����
				auto soulComp = soul->GetComponent<Soul>();
				if (soulComp != nullptr)
				{
					soulComp->SetSoulGauge(data.soulGauge);
					soulComp->SetMaxSoulGauge(data.maxSoulGauge);
				}

				mScene->AddGameObject(soul);
			}
		}

		// ���� �ش� ��ȥ�� ��ȯ�� �Ǿ����� ����
		mSoulSummonQueue.erase(std::remove_if(mSoulSummonQueue.begin(), mSoulSummonQueue.end()
			, [](const SoulData& data)
			{
				return data.durationTime >= SoulVariable::OutTime;
			}), mSoulSummonQueue.end());
	}

	void SoulManagerModule::SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab, bool isDestroy, float soulGauge, float maxSoulGauge)
	{
		SoulData data;

		data.id = id;
		data.soulType = soulType;
		data.worldTransform = worldTransform;
		data.soulPrefab = soulPrefab;
		data.soulGauge = soulGauge;
		data.maxSoulGauge = maxSoulGauge;

		if (!isDestroy)
			data.durationTime = SoulVariable::OutTime;

		// �ӽ� ������ ����
		data.tempGameObject = std::make_shared<fq::game_module::GameObject>();
		data.tempGameObject->GetTransform()->SetWorldMatrix(worldTransform);
		mScene->ViewComponents<CameraMoving>([data](game_module::GameObject& object, CameraMoving& camera)
			{
				camera.AddPlayerTransform(data.tempGameObject->GetComponent<game_module::Transform>());
			});

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

	float SoulManagerModule::GetDestoryArmourSoulDelayTime(unsigned int id)
	{
		for (auto& data : mSoulSummonQueue)
		{
			if (data.id == id)
				return data.durationTime;
		}

		return -1.f;
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
			if (id == 0)
			{
				SoulVariable::Player1DeathCount++;
			}
			else if (id == 1)
			{
				SoulVariable::Player2DeathCount++;
			}
			else if (id == 2)
			{
				SoulVariable::Player3DeathCount++;
			}
			else if (id == 3)
			{
				SoulVariable::Player4DeathCount++;
			}
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
			if (id == 0)
			{
				SoulVariable::Player1Type = playerType;
			}
			else if (id == 1)
			{
				SoulVariable::Player2Type = playerType;
			}
			else if (id == 2)
			{
				SoulVariable::Player3Type = playerType;
			}
			else if (id == 3)
			{
				SoulVariable::Player4Type = playerType;
			}
		}
	}
}
