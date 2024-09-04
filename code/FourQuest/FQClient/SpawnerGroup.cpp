#include "SpawnerGroup.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Transform.h"

#include "Spawner.h"
#include "ClientEvent.h"
#include "Monster.h"

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
	mMonsters()
{
}

fq::client::SpawnerGroup::SpawnerGroup(const SpawnerGroup& other)
{
	mID = other.mID;
	mStartNum = other.mStartNum;
	mSpawnConditions = other.mSpawnConditions;
	mSpawnRules = other.mSpawnRules;
	mMeleeMonsterPrefab = mMeleeMonsterPrefab;
	mExplosionMonsterPrefab = other.mExplosionMonsterPrefab;
	mBossMonsterPrefab = other.mBossMonsterPrefab;
	mPlantMonsterPrefab = other.mPlantMonsterPrefab;
	mSpawnerMonsterPrefab = other.mSpawnerMonsterPrefab;
	mCurRuleNum = other.mCurRuleNum;
	mCurSpawnMonsterNum = other.mCurSpawnMonsterNum;
	mMonsters = other.mMonsters;
}

fq::client::SpawnerGroup::~SpawnerGroup()
{
}

void fq::client::SpawnerGroup::OnStart()
{
	mMonsters.clear();
	EventProcessPlayerSpawnCollideTrigger();
}

void fq::client::SpawnerGroup::OnUpdate(float dt)
{
	// 각 조건 확인, SpawnCollider는 이벤트로 확인
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

	if (cloneSpawnerGroup == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSpawnerGroup = game_module::ObjectPool::GetInstance()->Assign<SpawnerGroup>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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

	return true;
}

void fq::client::SpawnerGroup::Spawn(SpawnRule rule)
{
	auto children = GetGameObject()->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		// 스폰하려는 스포너의 ID가 맞다면
		if (children[i]->GetComponent<Spawner>()->GetID() == rule.SpawnerNum)
		{
			// 몬스터 타입 결정
			game_module::PrefabResource mSelectPrefab;
			switch (rule.MonsterType)
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

			// Prefab으로 소환
			std::shared_ptr<game_module::GameObject> monster;

			for (int i = 0; i < rule.SpawnMonsterNum; i++)
			{
				auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSelectPrefab);
				monster = *(instance.begin());

				// monster 이름 변경
				monster->SetName(rule.Name + std::to_string(mCurSpawnMonsterNum));
				// Monster Spawner 위치로 이동
				monster->GetTransform()->SetLocalPosition(children[i]->GetTransform()->GetLocalPosition());

				GetScene()->AddGameObject(monster);
				mMonsters.emplace_back(monster);
				mCurSpawnMonsterNum++;
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

		// 여기랑 밑에 채워놓고 timer, collider 테스트 하기 
		std::vector<std::shared_ptr<game_module::GameObject>> targetOnObject{}; // 타겟이 null이 아니고 isdestroy가 아닌 것들
		std::copy_if(mMonsters.begin(), mMonsters.end(), std::back_inserter(targetOnObject),
			[this](std::shared_ptr<game_module::GameObject> monster)
			{
				auto target = monster->GetComponent<Monster>()->GetTarget();
				return !target->IsDestroyed();
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
	// 여기 채우기~
	// isLive 일 때랑 !isLive일 때랑 따로 처리하기 
	for (int i = 0; i < size; i++)
	{
		auto& objectLiveList = mSpawnConditions.ObjectLiveList[i];

		std::vector<std::shared_ptr<game_module::GameObject>> sameNameMonster{}; // 조건에 맞는 몬스터
		std::copy_if(mMonsters.begin(), mMonsters.end(), std::back_inserter(sameNameMonster),
			[this, objectLiveList](std::shared_ptr<game_module::GameObject> monster)
			{
				return monster->GetName().find(objectLiveList.ObjectOrPrefabName) != std::string::npos;
			}
		);

		// 살았는지 확인
		if (objectLiveList.isLive)
		{
			// 이름 일치하는 오브젝트 순회
			for (int i = 0; i < sameNameMonster.size(); i++)
			{
				if (!sameNameMonster[i]->IsDestroyed())
				{
					objectLiveList.isClear = true;
					continue;
				}
			}
		}
		// 죽었는지 확인
		else
		{
			// 이름 일치하는 오브젝트 순회
			for (int i = 0; i < sameNameMonster.size(); i++)
			{
				if (!sameNameMonster[i]->IsDestroyed())
				{
					continue;
				}
			}

			objectLiveList.isClear = true;
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
}

void fq::client::SpawnerGroup::EventProcessPlayerSpawnCollideTrigger()
{
	mPlayerSpawnCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
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
}

fq::client::SpawnerGroup& fq::client::SpawnerGroup::operator=(const SpawnerGroup& other)
{
	mID = other.mID;
	mStartNum = other.mStartNum;
	mSpawnConditions = other.mSpawnConditions;
	mSpawnRules = other.mSpawnRules;
	mMeleeMonsterPrefab = mMeleeMonsterPrefab;
	mExplosionMonsterPrefab = other.mExplosionMonsterPrefab;
	mBossMonsterPrefab = other.mBossMonsterPrefab;
	mPlantMonsterPrefab = other.mPlantMonsterPrefab;
	mSpawnerMonsterPrefab = other.mSpawnerMonsterPrefab;
	mCurRuleNum = other.mCurRuleNum;
	mCurSpawnMonsterNum = other.mCurSpawnMonsterNum;
	mMonsters = other.mMonsters;

	return *this;
}
