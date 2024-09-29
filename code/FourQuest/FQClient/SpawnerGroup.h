#pragma once

#include <vector>
#include <list>

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/EventHandler.h"

#include "SpawnStruct.h"

namespace fq::client
{
	class MonsterManager;

	class SpawnerGroup : public game_module::Component
	{
	public:
		SpawnerGroup();
		~SpawnerGroup();

		SpawnerGroup(const SpawnerGroup& other);
		SpawnerGroup& operator=(const SpawnerGroup& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnFixedUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		bool IsPassSpawnCondition();
		void Spawn(SpawnRule rule);
		void IsFinished(); // ����� ������ ����
		void InitCondition();

		void CheckTimer(int size, float dt);
		void CheckMaxEnemy(int size);
		void CheckObjectLive(int size);

		void EventProcessPlayerSpawnCollideTrigger();
		void EventProcessInProgressQuest();
		void EventProcessInProgressDefence();

	private:
		//�ν����� �� ����
		int mID;
		int mStartNum;

		SpawnCondition mSpawnConditions;
		SpawnRules mSpawnRules;

		game_module::PrefabResource mMeleeMonsterPrefab;
		game_module::PrefabResource mExplosionMonsterPrefab;
		game_module::PrefabResource mBossMonsterPrefab;
		game_module::PrefabResource mPlantMonsterPrefab;
		game_module::PrefabResource mSpawnerMonsterPrefab;

		// ���� ����
		int mCurRuleNum; 
		int mCurSpawnMonsterNum; // SpawnRule���� �̸� �����ٶ� �ڿ� ��ȣ ���� ���� 

		game_module::EventHandler mPlayerSpawnCollideTriggerHandler;
		game_module::EventHandler mInProgressQuestHandler;
		game_module::EventHandler mInProgressDefenceHandler;

		MonsterManager* mMonsterManager;

		std::list<std::shared_ptr<game_module::GameObject>> mAddedMonsterList; // �߰��ǰ� ���� ƽ�� Ÿ�� �����ϱ� ���ؼ� �ʿ�, �ٷδ� ��Ʈ�ѷ� ������ �ȵǾ �Ұ��� 

	private:
		friend void RegisterMetaData();
	};
}
