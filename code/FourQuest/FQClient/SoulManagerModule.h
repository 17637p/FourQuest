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
		float soulGauge = 0.f;
		float maxSoulGauge = 100.f;
	};

	class SoulManagerModule
	{
	public:
		SoulManagerModule();
		~SoulManagerModule();

		void AddPlayerArmourDeathCount(unsigned int id);
		void SetPlayerType(unsigned int id, const EPlayerType& playerType);

		ESoulType GetDestroyArmourSoulType(unsigned int id);
		float GetDestoryArmourSoulDelayTime(unsigned int id);

		/// <summary>
		/// 갑옷 파괴 및 갑옷 해체 시에 상태에 따라 영혼 생성
		/// </summary>
		void SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab, bool isDestroy, float soulGauge, float maxSoulGauge);

		/// <summary>
		/// 플레이어 상태 체크해서 현재 게임 오버 상태인지 체크
		/// </summary>
		bool CheckGameOver();

		void OnStart(fq::game_module::Scene* scene);
		void OnUpdate(float dt);

	private:
		void checkSummonSoul(float dt);

	private:
		fq::game_module::Scene* mScene;
		std::vector<SoulData> mSoulSummonQueue;

		friend void RegisterMetaData();
	};
}

