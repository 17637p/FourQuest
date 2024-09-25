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
		// 게임 오버인지 체크
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
		// 영혼이 소환 시간이 되었으면 씬에 영혼 생성
		for (auto& data : mSoulSummonQueue)
		{
			data.durationTime += dt;

			if (data.durationTime >= SoulVariable::OutTime)
			{
				auto instance = mScene->GetPrefabManager()->InstantiatePrefabResoure(data.soulPrefab);
				auto& soul = *(instance.begin());

				// 컨트롤러 연결
				soul->GetComponent<game_module::CharacterController>()
					->SetControllerID(data.id);

				// 소울 타입 설정
				soul->GetComponent<Soul>()->SetSoulType(data.soulType);

				// 위치 설정
				soul->GetComponent<game_module::Transform>()->SetWorldMatrix(data.worldTransform);

				mScene->AddGameObject(soul);
			}
		}

		// 만약 해당 영혼이 소환이 되었으면 삭제
		mSoulSummonQueue.erase(std::remove_if(mSoulSummonQueue.begin(), mSoulSummonQueue.end()
			, [](const SoulData& data)
			{
				return data.durationTime >= SoulVariable::OutTime;
			}), mSoulSummonQueue.end());
	}

	void SoulManagerModule::SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab, bool isDestroy)
	{
		SoulData data;

		data.id = id;
		data.soulType = soulType;
		data.worldTransform = worldTransform;
		data.soulPrefab = soulPrefab;

		if (!isDestroy)
			data.durationTime = SoulVariable::OutTime;

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
