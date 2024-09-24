#pragma once
#include "../FQGameModule/GameModule.h"
#include "PlayerDefine.h"

namespace fq::client
{
	struct SoulData
	{
		unsigned int id = 0;
		ESoulType soulType = ESoulType::End;
		DirectX::SimpleMath::Matrix worldTransform;
		game_module::PrefabResource soulPrefab;
		float durationTime = 0.f;
	};

	class SoulManagerModule
	{
	public:
		SoulManagerModule();
		~SoulManagerModule();

		int GetPlayerDeathCount(unsigned int id);
		EPlayerType GetPlayerType(unsigned int id);
		void AddPlayerArmourDeathCount(unsigned int id);
		void SetPlayerType(unsigned int id, const EPlayerType& playerType);

		ESoulType GetDestroyArmourSoulType(unsigned int id);

		void SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab);

		void Set(fq::game_module::Scene* scene);
		void OnStart();
		void OnUpdate(float dt);

	private:
		void checkGameOver();
		void checkSummonSoul(float dt);

	private:
		fq::game_module::Scene* mScene;

		std::vector<int> mPlayerArmourDeathCount;
		std::vector<EPlayerType> mPlayerType;
		std::vector<SoulData> mSoulSummonQueue;

		friend void RegisterMetaData();
	};
}

