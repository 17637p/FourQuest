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
		/// ���� �ı� �� ���� ��ü �ÿ� ���¿� ���� ��ȥ ����
		/// </summary>
		void SummonSoul(unsigned int id, ESoulType soulType, DirectX::SimpleMath::Matrix worldTransform, game_module::PrefabResource soulPrefab, bool isDestroy, float soulGauge, float maxSoulGauge);

		/// <summary>
		/// �÷��̾� ���� üũ�ؼ� ���� ���� ���� �������� üũ
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

