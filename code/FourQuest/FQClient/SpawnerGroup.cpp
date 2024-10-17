#include "SpawnerGroup.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Transform.h"

#include "Spawner.h"
#include "ClientEvent.h"
#include "MonsterManager.h"
#include "LevelHepler.h"

#include "MeleeMonster.h"
#include "BossMonster.h"
#include "PlantMonster.h"
#include "Player.h"

fq::client::SpawnerGroup::SpawnerGroup()
	:mID(0),
	mStartNum(0),
	mSpawnConditions(),
	mSpawnRules(),
	mMeleeMonsterPrefab(),
	mExplosionMonsterPrefab(),
	mBossMonsterPrefab(),
	mPlantMonsterPrefab(),
	mSpawnerMonsterPrefab(),
	mCurRuleNum(0),
	mCurSpawnMonsterNum(0),
	mMonsterManager(nullptr)
{
}

fq::client::SpawnerGroup::SpawnerGroup(const SpawnerGroup& other)
{
	mID = other.mID;
	mStartNum = other.mStartNum;
	mSpawnConditions = other.mSpawnConditions;
	mSpawnRules = other.mSpawnRules;
	mMeleeMonsterPrefab = other.mMeleeMonsterPrefab;
	mExplosionMonsterPrefab = other.mExplosionMonsterPrefab;
	mBossMonsterPrefab = other.mBossMonsterPrefab;
	mPlantMonsterPrefab = other.mPlantMonsterPrefab;
	mSpawnerMonsterPrefab = other.mSpawnerMonsterPrefab;
	mCurRuleNum = other.mCurRuleNum;
	mCurSpawnMonsterNum = other.mCurSpawnMonsterNum;
	mMonsterManager = other.mMonsterManager;
}

fq::client::SpawnerGroup::~SpawnerGroup()
{
}

void fq::client::SpawnerGroup::OnStart()
{
	mMonsterManager = GetScene()->GetObjectByName("MonsterManager")->GetComponent<MonsterManager>();
	EventProcessPlayerSpawnCollideTrigger();
	EventProcessInProgressQuest();
	EventProcessInProgressDefence();
}

void fq::client::SpawnerGroup::OnUpdate(float dt)
{
	// �� ���� Ȯ��, SpawnCollider�� �̺�Ʈ�� Ȯ��
	int maxEnemyListSize = mSpawnConditions.MaxEnemyList.size();
	if (maxEnemyListSize > 0)
	{
		CheckMaxEnemy(maxEnemyListSize);
	}
	int timerListSize = mSpawnConditions.TimerList.size();
	if (timerListSize > 0)
	{
		CheckTimer(timerListSize, dt);
	}
	int objectLiveListSize = mSpawnConditions.ObjectLiveList.size();
	if (objectLiveListSize > 0)
	{
		CheckObjectLive(objectLiveListSize);
	}

	if (IsPassSpawnCondition())
	{
		InitCondition();
		Spawn(mSpawnRules.SpawnRuleList[mCurRuleNum]);
	}
}

std::shared_ptr<fq::game_module::Component> fq::client::SpawnerGroup::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpawnerGroup = std::dynamic_pointer_cast<SpawnerGroup>(clone);

	if (cloneSpawnerGroup == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSpawnerGroup = game_module::ObjectPool::GetInstance()->Assign<SpawnerGroup>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSpawnerGroup = *this;
	}

	return cloneSpawnerGroup;
}

bool fq::client::SpawnerGroup::IsPassSpawnCondition()
{
	for (int i = 0; i < mSpawnConditions.SpawnColliderTriggerList.size(); i++)
	{
		if (!mSpawnConditions.SpawnColliderTriggerList[i].isClear)
		{
			return false;
		}
	}
	for (int i = 0; i < mSpawnConditions.MaxEnemyList.size(); i++)
	{
		if (!mSpawnConditions.MaxEnemyList[i].isClear)
		{
			return false;
		}
	}
	for (int i = 0; i < mSpawnConditions.TimerList.size(); i++)
	{
		if (!mSpawnConditions.TimerList[i].isClear)
		{
			return false;
		}
	}
	for (int i = 0; i < mSpawnConditions.ObjectLiveList.size(); i++)
	{
		if (!mSpawnConditions.ObjectLiveList[i].isClear)
		{
			return false;
		}
	}
	for (int i = 0; i < mSpawnConditions.InProgressQuestList.size(); i++)
	{
		if (!mSpawnConditions.InProgressQuestList[i].isClear)
		{
			return false;
		}
	}
	for (int i = 0; i < mSpawnConditions.InProgressDefenceList.size(); i++)
	{
		if (!mSpawnConditions.InProgressDefenceList[i].isClear)
		{
			return false;
		}
	}

	return true;
}

void fq::client::SpawnerGroup::Spawn(SpawnRule rule)
{
	auto children = GetGameObject()->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		for (int k = 0; k < rule.spawnData.size(); k++)
		{
			// �����Ϸ��� �������� ID�� �´ٸ�
			if (children[i]->GetComponent<Spawner>()->GetID() == rule.spawnData[k].SpawnerNum)
			{
				// ���� Ÿ�� ����
				game_module::PrefabResource mSelectPrefab;
				switch (rule.spawnData[k].MonsterType)
				{
					case EMonsterType::Melee:
						mSelectPrefab = mMeleeMonsterPrefab;
						break;
					case EMonsterType::Plant:
						mSelectPrefab = mPlantMonsterPrefab;
						break;
					case EMonsterType::Explosion:
						mSelectPrefab = mExplosionMonsterPrefab;
						break;
					case EMonsterType::Boss:
						mSelectPrefab = mBossMonsterPrefab;
						break;
					case EMonsterType::Spawner:
						mSelectPrefab = mSpawnerMonsterPrefab;
						break;
					default:
						break;
				}

				// Prefab���� ��ȯ
				std::shared_ptr<game_module::GameObject> monster;
				int spawnMonsterNum = LevelHepler::GetSpawnRatio() * rule.spawnData[k].SpawnMonsterNum;
				for (int j = 0; j < spawnMonsterNum; j++)
				{
					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSelectPrefab);
					monster = *(instance.begin());

					// monster �̸� ����
					monster->SetName(rule.spawnData[k].Name + std::to_string(mCurSpawnMonsterNum));
					
					// Monster Spawner ��ġ�� �̵�
					DirectX::SimpleMath::Vector3 spawnPos = children[i]->GetTransform()->GetWorldPosition();
					monster->GetTransform()->SetWorldPosition(spawnPos);
					mAddedMonsterList.push_back(monster);

					GetScene()->AddGameObject(monster);
					mCurSpawnMonsterNum++;
				}

				break;
			}
		}
	}

	IsFinished();
}

void fq::client::SpawnerGroup::IsFinished()
{
	mCurRuleNum++;
	if (mCurRuleNum >= mSpawnRules.SpawnRuleList.size())
	{
		if (mSpawnRules.isLoof)
		{
			mCurRuleNum = 0;
		}
		else
		{
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}

void fq::client::SpawnerGroup::CheckTimer(int size, float dt)
{
	for (int i = 0; i < size; i++)
	{
		auto& timerList = mSpawnConditions.TimerList[i];

		if (timerList.isClear)
		{
			continue;
		}

		timerList.curSecond += dt;
		if (timerList.curSecond > timerList.Second)
		{
			timerList.isClear = true;
		}
	}
}

void fq::client::SpawnerGroup::CheckMaxEnemy(int size)
{
	for (int i = 0; i < size; i++)
	{
		auto& maxEnemyList = mSpawnConditions.MaxEnemyList[i];

		if (maxEnemyList.isClear)
		{
			continue;
		}

		// ����� �ؿ� ä������ timer, collider �׽�Ʈ �ϱ� 
		std::vector<std::shared_ptr<game_module::GameObject>> targetOnObject{}; // Ÿ���� null�� �ƴϰ� isdestroy�� �ƴ� �͵�
		auto monsters = mMonsterManager->GetMonsters();
		std::copy_if(monsters.begin(), monsters.end(), std::back_inserter(targetOnObject),
			[this](std::shared_ptr<game_module::GameObject> monster)
			{
				auto isMelee = monster->GetComponent<MeleeMonster>();
				std::shared_ptr<game_module::GameObject> target;
				if (isMelee)
				{
					target = isMelee->GetTarget();
				}
				auto isBoss = monster->GetComponent<BossMonster>();
				if (isBoss)
				{
					target = isBoss->GetTarget();
				}
				auto isPlant = monster->GetComponent<PlantMonster>();
				if (isPlant)
				{
					target = isPlant->GetTarget();
				}

				if (target)
				{
					return !target->IsDestroyed();
				}
				else
				{
					return false;
				}
			}
		);

		switch (maxEnemyList.ComparisonOperator)
		{
			case EComparisonOperators::Equals:
				if (targetOnObject.size() == maxEnemyList.MonsterNum)
				{
					maxEnemyList.isClear = true;
				}
				break;
			case EComparisonOperators::Greater:
				if (targetOnObject.size() > maxEnemyList.MonsterNum)
				{
					maxEnemyList.isClear = true;
				}
				break;
			case EComparisonOperators::Less:
				if (targetOnObject.size() < maxEnemyList.MonsterNum)
				{
					maxEnemyList.isClear = true;
				}
				break;
			default:
				break;
		}
	}
}

void fq::client::SpawnerGroup::CheckObjectLive(int size)
{
	// ���� ä���~
	// isLive �� ���� !isLive�� ���� ���� ó���ϱ� 
	for (int i = 0; i < size; i++)
	{
		auto& objectLiveList = mSpawnConditions.ObjectLiveList[i];

		if (objectLiveList.isClear)
		{
			continue;
		}

		std::vector<std::shared_ptr<game_module::GameObject>> sameNameMonster{}; // ���ǿ� �´� ����
		auto monsters = mMonsterManager->GetMonsters();
		std::copy_if(monsters.begin(), monsters.end(), std::back_inserter(sameNameMonster),
			[this, objectLiveList](std::shared_ptr<game_module::GameObject> monster)
			{
				return monster->GetName().find(objectLiveList.ObjectOrPrefabName) != std::string::npos;
			}
		);

		// ��Ҵ��� Ȯ��
		if (objectLiveList.isLive)
		{
			// �̸� ��ġ�ϴ� ������Ʈ ��ȸ
			for (int i = 0; i < sameNameMonster.size(); i++)
			{
				if (!sameNameMonster[i]->IsDestroyed())
				{
					objectLiveList.isClear = true;
					continue;
				}
			}
		}
		// �׾����� Ȯ��
		else
		{
			bool isDie = true;
			// �̸� ��ġ�ϴ� ������Ʈ ��ȸ
			for (int i = 0; i < sameNameMonster.size(); i++)
			{
				if (!sameNameMonster[i]->IsDestroyed())
				{
					isDie = false;
					break;
				}
			}

			if (isDie)
			{
				objectLiveList.isClear = true;
			}
		}
	}
}

void fq::client::SpawnerGroup::InitCondition()
{
	auto& spawnCollidertriggerList = mSpawnConditions.SpawnColliderTriggerList;
	for (int i = 0; i < spawnCollidertriggerList.size(); i++)
	{
		spawnCollidertriggerList[i].isClear = false;
	}
	auto& maxEnemyList = mSpawnConditions.MaxEnemyList;
	for (int i = 0; i < maxEnemyList.size(); i++)
	{
		maxEnemyList[i].isClear = false;
	}
	auto& timerList = mSpawnConditions.TimerList;
	for (int i = 0; i < timerList.size(); i++)
	{
		timerList[i].isClear = false;
		timerList[i].curSecond = 0;
	}
	auto& objectLiveList = mSpawnConditions.ObjectLiveList;
	for (int i = 0; i < objectLiveList.size(); i++)
	{
		objectLiveList[i].isClear = false;
	}
	auto& inProgressQuestList = mSpawnConditions.InProgressQuestList;
	for (int i = 0; i < inProgressQuestList.size(); i++)
	{
		inProgressQuestList[i].isClear = false;
	}
	auto& inProgressDefenceList = mSpawnConditions.InProgressDefenceList;
	for (int i = 0; i < inProgressDefenceList.size(); i++)
	{
		inProgressDefenceList[i].isClear = false;
	}
}

// 09.20 Enter -> Stay �̺�Ʈ�� ����
void fq::client::SpawnerGroup::EventProcessPlayerSpawnCollideTrigger()
{
	mPlayerSpawnCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideStayTrigger>(
		[this](const client::event::PlayerCollideStayTrigger& event)
		{
			for (int i = 0; i < mSpawnConditions.SpawnColliderTriggerList.size(); i++)
			{
				auto& spawnColliderTriggerList = mSpawnConditions.SpawnColliderTriggerList[i];
				if (event.colliderName == spawnColliderTriggerList.ColliderName)
				{
					spawnColliderTriggerList.isClear = true;
				}
			}
		});
}

void fq::client::SpawnerGroup::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mPlayerSpawnCollideTriggerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mInProgressQuestHandler);
	GetScene()->GetEventManager()->RemoveHandle(mInProgressDefenceHandler);
}

fq::client::SpawnerGroup& fq::client::SpawnerGroup::operator=(const SpawnerGroup& other)
{
	mID = other.mID;
	mStartNum = other.mStartNum;
	mSpawnConditions = other.mSpawnConditions;
	mSpawnRules = other.mSpawnRules;
	mMeleeMonsterPrefab = other.mMeleeMonsterPrefab;
	mExplosionMonsterPrefab = other.mExplosionMonsterPrefab;
	mBossMonsterPrefab = other.mBossMonsterPrefab;
	mPlantMonsterPrefab = other.mPlantMonsterPrefab;
	mSpawnerMonsterPrefab = other.mSpawnerMonsterPrefab;
	mCurRuleNum = other.mCurRuleNum;
	mCurSpawnMonsterNum = other.mCurSpawnMonsterNum;
	mMonsterManager = other.mMonsterManager;

	return *this;
}

void fq::client::SpawnerGroup::OnFixedUpdate(float dt)
{
	for (auto& monster : mAddedMonsterList)
	{
		// Target ���� ����
		game_module::GameObject* nearestPlayer = nullptr;
		float minDistance = FLT_MAX;

		for (auto& player : GetScene()->GetComponentView<Player>())
		{
			DirectX::SimpleMath::Vector3 playerPos = player.GetTransform()->GetWorldPosition();
			DirectX::SimpleMath::Vector3 monsterPos = monster->GetTransform()->GetWorldPosition();
			float distance = DirectX::SimpleMath::Vector3::Distance(playerPos, monsterPos);
			if (distance < minDistance)
			{
				minDistance = distance;
				nearestPlayer = &player;
			}
		}

		auto isMelee = monster->GetComponent<MeleeMonster>();
		auto isBoss = monster->GetComponent<BossMonster>();
		auto isPlant = monster->GetComponent<PlantMonster>();
		std::shared_ptr<game_module::GameObject> target;
		if (isMelee)
		{
			isMelee->SetTarget(nearestPlayer);
		}
		else if (isBoss)
		{
			isBoss->SetTarget(nearestPlayer);
		}
		else if (isPlant)
		{
			isPlant->SetTarget(nearestPlayer);
			isPlant->CheckTargetInAttackRange();
		}

		// �߰�_ȫ��ȯ
		// �����׷��� ���� ���� �� �ִϸ��̼��� �����Ű�� ���� �߰�
		auto animator = monster->GetComponent<fq::game_module::Animator>();
		if (animator != nullptr)
		{
			animator->SetParameterBoolean("OnSpawnEvent", true);
		}
	}

	mAddedMonsterList.clear();
}

void fq::client::SpawnerGroup::EventProcessInProgressQuest()
{
	mInProgressQuestHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::CurrentQuest>(
		[this](const client::event::CurrentQuest& event)
		{
			for (int i = 0; i < mSpawnConditions.InProgressQuestList.size(); i++)
			{
				auto& inProgressQuestList = mSpawnConditions.InProgressQuestList[i];
				if (event.isMain == inProgressQuestList.isMain && event.questIndex == inProgressQuestList.QuestIndex)
				{
					inProgressQuestList.isClear = true;
				}
			}
		});
}

void fq::client::SpawnerGroup::EventProcessInProgressDefence()
{
	mInProgressDefenceHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::InProgressDefence>(
		[this](const client::event::InProgressDefence& event)
		{
			for (int i = 0; i < mSpawnConditions.InProgressDefenceList.size(); i++)
			{
				auto& inProgressDefenceList = mSpawnConditions.InProgressDefenceList[i];
				if (event.colliderName == inProgressDefenceList.ColliderName && event.curCount >= inProgressDefenceList.Count)
				{
					inProgressDefenceList.isClear = true;
				}
			}
		});
}
