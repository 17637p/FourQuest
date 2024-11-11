#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

namespace fq::client
{
	class DeadArmour : public game_module::Component
	{
	public:
		DeadArmour();
		~DeadArmour();
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	public:
		/// <summary>
		/// �÷��̾� ��ȯ ������ ���ϴ�. 
		/// Ÿ�̸Ӹ�ŭ ����ؾ���
		/// </summary>
		bool EnterSummonLivingArmour(PlayerInfo info);

		/// <summary>
		/// ���� ���ʿ��� �÷��̾ ��ȯ�մϴ�
		/// </summary>
		bool SummonLivingArmour(PlayerInfo info);

		void SetUnequippedPlayerId(int playerID) { mUnequippedPlayerId = playerID; }
		int GetUnequippedPlayerId() { return mUnequippedPlayerId; }

		float GetPlayerArmourCoolTime() { return mPlayerArmourCoolTime; }

		/// <summary>
		/// ������ ���� ������ �������� �����մϴ� 
		/// </summary>
		void SetSummonAble(bool isSummonAble);

		bool IsSummonAble()const;

		/// <summary>
		/// �÷��̾ ������ ���� �� �ִ� �� ��Ÿ���� Ȯ���ϰ� UI�� ǥ���մϴ�.
		/// </summary>
		void CheckArmourCoolTime(unsigned int playerID) const;

		// ���â �� 
		void AddArmourCount(int playerID);

		void SetHp(float hp);

		float GetHp() const;

		// ���ʿ� ���� ���� ������ ����
		bool IsOnSummon() const;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		void setOutlineColor(DirectX::SimpleMath::Color color);
		void setUI(bool isVisible);
	private:
		game_module::PrefabResource mLivingArmourPrefab;

		unsigned short mSoulCount;
		unsigned short mPlayerCount;
		bool mbIsVisible;
		bool mbIsSummonAble;

		int mUnequippedPlayerId;
		float mPlayerArmourCoolTime;

		float mSummonDuration;
		bool mbIsOnSummon;
		float mSummonElapsedTime;

		float mHp;

		DirectX::SimpleMath::Vector2 mUIOffset;

		friend void RegisterMetaData();
	};
}