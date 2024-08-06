#include "QuestManager.h"
#include "ClientEvent.h"

#include "DefenceCounter.h"

#include <spdlog/spdlog.h>

fq::client::QuestManager::QuestManager()
	:mStartQuests(),
	mMainQuests(),
	mSubQuests(),
	mCurMainQuest(),
	mCurSubQuest()
{
}

fq::client::QuestManager::QuestManager(const QuestManager& other)
	:mStartQuests(other.mStartQuests),
	mMainQuests(other.mMainQuests),
	mSubQuests(other.mSubQuests),
	mCurMainQuest(other.mCurMainQuest),
	mCurSubQuest(other.mCurSubQuest)
{
}

fq::client::QuestManager& fq::client::QuestManager::operator=(const QuestManager& other)
{
	mStartQuests = other.mStartQuests;
	mMainQuests = other.mMainQuests;
	mSubQuests = other.mSubQuests;
	mCurMainQuest = other.mCurMainQuest;
	mCurSubQuest = other.mCurSubQuest;

	return *this;
}

fq::client::QuestManager::~QuestManager()
{
}

std::shared_ptr<fq::game_module::Component> fq::client::QuestManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneQuestManager = std::dynamic_pointer_cast<QuestManager>(clone);

	if (cloneQuestManager == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneQuestManager = game_module::ObjectPool::GetInstance()->Assign<QuestManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneQuestManager = *this;
	}

	return cloneQuestManager;
}

void fq::client::QuestManager::OnStart()
{
	mCurSubQuest.clear();

	mCurMainQuest = mMainQuests[mStartQuests.startMainQuestIndex];
	for (int i = 0; i < mStartQuests.startSubQuestIndex.size(); i++)
	{
		auto it = std::find_if(mSubQuests.begin(), mSubQuests.end(), [this, i](Quest quest)
			{
				return quest.mIndex == mStartQuests.startSubQuestIndex[i].index;
			});
		mCurSubQuest.push_back(*it);
	}

	// 이벤트 핸들러 등록
	EventProcessKillMonster();
	EventProcessPlayerCollideTrigger();
	EventProcessCompleteDefence();
	EventProcessClearQuest();
	EventProcessAllColliderTrigger();
}

void fq::client::QuestManager::OnUpdate(float dt)
{
	// Quest Clear
	//if()
	int a = 3;
}

void fq::client::QuestManager::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mKillMonsterHandler);
	GetScene()->GetEventManager()->RemoveHandle(mPlayerCollideTriggerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mCompleteDefenceHandler);
	GetScene()->GetEventManager()->RemoveHandle(mClearQuestHandler);
	GetScene()->GetEventManager()->RemoveHandle(mAllCollideTriggerHandler);
}

void fq::client::QuestManager::EventProcessKillMonster()
{
	mKillMonsterHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::KillMonster>(
		[this](const client::event::KillMonster& event)
		{
			// Main
			std::vector<MonsterKill> monsterKillList = mCurMainQuest.mclearConditionList.monsterKillList;
			int monsterKillListSize = monsterKillList.size();
			if (monsterKillListSize > 0)
			{
				bool isClear = true;
				for (int i = 0; i < monsterKillListSize; i++)
				{
					if (monsterKillList[i].monsterType == event.monsterType)
					{
						monsterKillList[i].curNumber++;
						spdlog::trace("curNumber: {}, requestNumber: {}", monsterKillList[i].curNumber, monsterKillList[i].requestsNumber);

						if (monsterKillList[i].curNumber == monsterKillList[i].requestsNumber)
						{
							monsterKillList[i].isClear = true;
						}
					}

					if (!monsterKillList[i].isClear)
					{
						isClear = false;
					}
				}

				if (isClear)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("MonsterKill Quest Clear!");
				}
			}

			// Sub
			for (int j = 0; j < mCurSubQuest.size(); j++)
			{
				std::vector<MonsterKill> monsterKillList = mCurSubQuest[j].mclearConditionList.monsterKillList;
				int monsterKillListSize = monsterKillList.size();
				if (monsterKillListSize > 0)
				{
					bool isClear = true;
					for (int i = 0; i < monsterKillListSize; i++)
					{
						if (monsterKillList[i].monsterType == event.monsterType)
						{
							monsterKillList[i].curNumber++;
							spdlog::trace("curNumber: {}, requestNumber: {}", monsterKillList[i].curNumber, monsterKillList[i].requestsNumber);

							if (monsterKillList[i].curNumber == monsterKillList[i].requestsNumber)
							{
								monsterKillList[i].isClear = true;
							}
						}

						if (!monsterKillList[i].isClear)
						{
							isClear = false;
						}
					}

					if (isClear)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[j], j});
						spdlog::trace("MonsterKill Quest Clear!");
					}
				}
			}
		});
}

void fq::client::QuestManager::EventProcessPlayerCollideTrigger()
{
	// Defence 처리
	mPlayerCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
		{
			std::vector<Defence> defenceList = mCurMainQuest.mclearConditionList.defenceList;
			if (defenceList.size() > 0)
			{
				if (defenceList[0].colliderName == event.colliderName)
				{
					GetScene()->GetObjectByName(event.colliderName)->GetComponent<DefenceCounter>()->SetIsStart(true);
					spdlog::trace("Defence Quest Start!");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<Defence> defenceList = mCurSubQuest[i].mclearConditionList.defenceList;
				if (defenceList.size() > 0)
				{
					if (defenceList[0].colliderName == event.colliderName)
					{
						GetScene()->GetObjectByName(event.colliderName)->GetComponent<DefenceCounter>()->SetIsStart(true);
						spdlog::trace("Defence Quest Start!");
					}
				}
			}
		});

	// Join - Collider Trigger 처리
	mPlayerCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
		{
			std::vector<QuestColliderTrigger> questColliderTriggerList = mCurMainQuest.mJoinConditionList.colliderTriggerList;
			if (questColliderTriggerList.size() > 0)
			{
				if (questColliderTriggerList[0].colliderName == event.colliderName)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Complete Collider Trigger Clear Condition");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger> questColliderTriggerList = mCurSubQuest[i].mJoinConditionList.colliderTriggerList;
				if (questColliderTriggerList.size() > 0)
				{
					if (questColliderTriggerList[0].colliderName == event.colliderName)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Complete Collider Trigger Clear Condition");
					}
				}
			}
		});

	// Clear - Collider Trigger 처리
	mPlayerCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
		{
			std::vector<QuestColliderTrigger> questColliderTriggerList = mCurMainQuest.mclearConditionList.colliderTriggerList;
			if (questColliderTriggerList.size() > 0)
			{
				if (questColliderTriggerList[0].colliderName == event.colliderName)
				{
					if (!questColliderTriggerList[0].isAll)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurMainQuest, 0 });
						spdlog::trace("Complete Collider Trigger Single Clear Condition");
					}
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger> questColliderTriggerList = mCurSubQuest[i].mclearConditionList.colliderTriggerList;
				if (questColliderTriggerList.size() > 0)
				{
					if (questColliderTriggerList[0].colliderName == event.colliderName)
					{
						if (!questColliderTriggerList[0].isAll)
						{
							GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
								{ mCurSubQuest[i], i });
							spdlog::trace("Complete Collider Trigger Single Clear Condition");
						}
					}
				}
			}
		});
}

void fq::client::QuestManager::EventProcessCompleteDefence()
{
	mCompleteDefenceHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::CompleteDefence>(
		[this](const client::event::CompleteDefence& event)
		{
			std::vector<Defence> defenceList = mCurMainQuest.mclearConditionList.defenceList;
			if (defenceList.size() > 0)
			{
				if (defenceList[0].colliderName == event.colliderName)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Defence Quest Clear!");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<Defence> defenceList = mCurSubQuest[i].mclearConditionList.defenceList;
				if (defenceList.size() > 0)
				{
					if (defenceList[0].colliderName == event.colliderName)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Defence Quest Clear!");
					}
				}
			}
		});
}

void fq::client::QuestManager::EventProcessClearQuest()
{
	mClearQuestHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ClearQuestEvent>(
		[this](const client::event::ClearQuestEvent& event)
		{
			// 방금 깬 퀘스트가 선행퀘스트라면 다음 퀘스트 추가하기
			for (int i = 0; i < mMainQuests.size(); i++)
			{
				std::vector<PreQuest> preQuestList = mMainQuests[i].mJoinConditionList.preQuestList;
				if (preQuestList.size() > 0)
				{
					if (preQuestList[0].preIndex == event.clearQuest.mIndex &&
						preQuestList[0].preIsMain == event.clearQuest.mIsMain)
					{
						mCurMainQuest = mMainQuests[i];
						spdlog::trace("Complete PreQuest!");
					}
				}
			}
			for (int i = 0; i < mSubQuests.size(); i++)
			{
				std::vector<PreQuest> preQuestList = mSubQuests[i].mJoinConditionList.preQuestList;
				if (preQuestList.size() > 0)
				{
					if (preQuestList[0].preIndex == event.clearQuest.mIndex &&
						preQuestList[0].preIsMain == event.clearQuest.mIsMain)
					{
						mCurSubQuest[event.index] = mSubQuests[i];
						spdlog::trace("Complete PreQuest!");
					}
				}
			}

			// 방금 깬 퀘스트가 클리어 조건으로 있는 퀘스트가 있다면 클리어 하기 
			std::vector<ClearQuest> clearQuestList = mCurMainQuest.mclearConditionList.clearQuestList;
			if (clearQuestList.size() > 0)
			{
				if (clearQuestList[0].clearIndex == event.clearQuest.mIndex &&
					clearQuestList[0].clearIsMain == event.clearQuest.mIsMain)
				{
					spdlog::trace("Clear Quest Condition True!");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<ClearQuest> clearQuestList = mCurSubQuest[i].mclearConditionList.clearQuestList;
				if (clearQuestList.size() > 0)
				{
					if (clearQuestList[0].clearIndex == event.clearQuest.mIndex &&
						clearQuestList[0].clearIsMain == event.clearQuest.mIsMain)
					{
						spdlog::trace("Clear Quest Condition True!");
					}
				}
			}
		});
}

void fq::client::QuestManager::EventProcessAllColliderTrigger()
{
	// Clear - Collider Trigger 처리
	mAllCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::AllCollideTrigger>(
		[this](const client::event::AllCollideTrigger& event)
		{
			std::vector<QuestColliderTrigger> questColliderTriggerList = mCurMainQuest.mclearConditionList.colliderTriggerList;
			if (questColliderTriggerList.size() > 0)
			{
				if (questColliderTriggerList[0].colliderName == event.colliderName)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Complete Collider Trigger All Clear Condition");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger> questColliderTriggerList = mCurSubQuest[i].mclearConditionList.colliderTriggerList;
				if (questColliderTriggerList.size() > 0)
				{
					if (questColliderTriggerList[0].colliderName == event.colliderName)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Complete Collider Trigger All Clear Condition");
					}
				}
			}
		});
}

void fq::client::QuestManager::EventProcessObjectInteraction()
{
	mObjectInteractionHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ObjectInteractionEvent>(
		[this](const client::event::ObjectInteractionEvent& event)
		{
			std::vector<ObjectInteraction> objectInteractionList = mCurMainQuest.mclearConditionList.objectInteration;
			if (objectInteractionList.size() > 0)
			{
				if (objectInteractionList[0].tag == event.tag)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Clear Interaction Main Quest");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<ObjectInteraction> objectInteractionList = mCurSubQuest[i].mclearConditionList.objectInteration;
				if (objectInteractionList.size() > 0)
				{
					if (objectInteractionList[0].tag == event.tag)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Clear Interaction Sub Quest");
					}
				}
			}
		});
}

