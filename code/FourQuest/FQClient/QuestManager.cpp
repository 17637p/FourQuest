#include "QuestManager.h"
#include "ClientEvent.h"

#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Sequence.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/NavigationAgent.h"

#include "DefenceCounter.h"
#include "MonsterGroup.h"
#include "Portal.h"
#include "ArmourSet.h"
#include "CameraMoving.h"

#include <spdlog/spdlog.h>

#include <random>
#include <chrono>

fq::client::QuestManager::QuestManager()
	:mStartQuests(),
	mMainQuests(),
	mSubQuests(),
	mCurMainQuest(),
	mCurSubQuest(),
	mGaugeMaxWidth(0),
	mPortalPrefab(),
	mAddedArmourObjects(),
	mDistance(5)
{
}

fq::client::QuestManager::QuestManager(const QuestManager& other)
	:mStartQuests(other.mStartQuests),
	mMainQuests(other.mMainQuests),
	mSubQuests(other.mSubQuests),
	mCurMainQuest(other.mCurMainQuest),
	mCurSubQuest(other.mCurSubQuest),
	mPortalPrefab(other.mPortalPrefab),
	mDistance(other.mDistance)
{
}

fq::client::QuestManager& fq::client::QuestManager::operator=(const QuestManager& other)
{
	mStartQuests = other.mStartQuests;
	mMainQuests = other.mMainQuests;
	mSubQuests = other.mSubQuests;
	mCurMainQuest = other.mCurMainQuest;
	mCurSubQuest = other.mCurSubQuest;
	mPortalPrefab = other.mPortalPrefab;
	mDistance = other.mDistance;

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

	cloneQuestManager->mSubQuestTexts.clear();
	return cloneQuestManager;
}

void fq::client::QuestManager::OnStart()
{
	for (int i = 0; i < mMainQuests.size(); i++)
	{
		mMainQuests[i].mIsMain = true;
	}
	for (int i = 0; i < mSubQuests.size(); i++)
	{
		mSubQuests[i].mIsMain = false;
	}

	// 시작 퀘스트 등록
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

	// 
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren();
	mMainQuestText = children[1]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	for (int i = 0; i < 3; i++)
	{
		mSubQuestTexts.push_back(children[3]->GetChildren()[i]->GetChildren()[0]->GetComponent<game_module::TextUI>());
	}

	// GaugeMaxWidth 설정
	mGaugeMaxWidth = mMainQuestText->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>()->GetUIInfomation(0).Width;
	// 기본 FontSize 설정
	mFontSize = mMainQuestText->GetTextInfo().FontSize;

	// 이벤트 핸들러 등록
	EventProcessKillMonster();
	EventProcessPlayerCollideTrigger();
	EventProcessCompleteDefence();
	EventProcessClearQuest();
	EventProcessAllColliderTrigger();
	EventProcessObjectInteraction();

	mScreenManager = GetScene()->GetScreenManager();

	for (int i = 0; i < 3 - mCurSubQuest.size(); i++)
	{
		RenderOnSubQuest(2 - i, false);
	}
}

void fq::client::QuestManager::OnUpdate(float dt)
{
	SetScaleAndPositionScreen();

	// Main Quest Text Setting
	auto textInfo = mMainQuestText->GetTextInfo();
	textInfo.Text = mCurMainQuest.mName;
	mMainQuestText->SetTextInfo(textInfo);

	ViewQuestInformation(mCurMainQuest, mMainQuestText);

	// Sub Quest Text Setting
	for (int i = 0; i < mCurSubQuest.size(); i++)
	{
		auto textInfo = mSubQuestTexts[i]->GetTextInfo();
		textInfo.Text = mCurSubQuest[i].mName;
		mSubQuestTexts[i]->SetTextInfo(textInfo);
		ViewQuestInformation(mCurSubQuest[i], mSubQuestTexts[i]);
	}
}

void fq::client::QuestManager::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mKillMonsterHandler);
	GetScene()->GetEventManager()->RemoveHandle(mPlayerCollideTriggerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mCompleteDefenceHandler);
	GetScene()->GetEventManager()->RemoveHandle(mClearQuestHandler);
	GetScene()->GetEventManager()->RemoveHandle(mAllCollideTriggerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mObjectInteractionHandler);
}

void fq::client::QuestManager::EventProcessKillMonster()
{
	mKillMonsterHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::KillMonster>(
		[this](const client::event::KillMonster& event)
		{
			// KillMonster 처리 
			// Main
			std::vector<MonsterKill>& monsterKillList = mCurMainQuest.mclearConditionList.monsterKillList;
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
				std::vector<MonsterKill>& monsterKillList = mCurSubQuest[j].mclearConditionList.monsterKillList;
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

			// Group Monster Kill 처리
			// Main
			std::vector<MonsterGroupKill>& monsterGroupKillList = mCurMainQuest.mclearConditionList.monsterGroupKillList;
			if (monsterGroupKillList.size() > 0)
			{
				MonsterGroup* monsterGroup = GetScene()->GetObjectByName(monsterGroupKillList[0].monsterGroupName)->GetComponent<MonsterGroup>();
				monsterGroupKillList[0].groupMonsterNumber = monsterGroup->GetAllMonsterSize();
				monsterGroupKillList[0].curNumber = monsterGroup->GetRemainMonsterSize();
			}

			// Sub
			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<MonsterGroupKill>& monsterGroupKillList = mCurSubQuest[i].mclearConditionList.monsterGroupKillList;
				if (monsterGroupKillList.size() > 0)
				{
					MonsterGroup* monsterGroup = GetScene()->GetObjectByName(monsterGroupKillList[0].monsterGroupName)->GetComponent<MonsterGroup>();
					monsterGroupKillList[0].groupMonsterNumber = monsterGroup->GetAllMonsterSize();
					monsterGroupKillList[0].curNumber = monsterGroup->GetRemainMonsterSize();
				}
			}
		});
}

void fq::client::QuestManager::EventProcessPlayerCollideTrigger()
{
	mPlayerCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
		{
			// Defence 처리
			std::vector<Defence>& defenceList = mCurMainQuest.mclearConditionList.defenceList;
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
				std::vector<Defence>& defenceList = mCurSubQuest[i].mclearConditionList.defenceList;
				if (defenceList.size() > 0)
				{
					if (defenceList[0].colliderName == event.colliderName)
					{
						GetScene()->GetObjectByName(event.colliderName)->GetComponent<DefenceCounter>()->SetIsStart(true);
						spdlog::trace("Defence Quest Start!");
					}
				}
			}

			// Join - Collider Trigger 처리
			std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mCurMainQuest.mJoinConditionList.colliderTriggerList;
			if (joinQuestColliderTriggerList.size() > 0)
			{
				if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Complete Collider Trigger Clear Condition");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mCurSubQuest[i].mJoinConditionList.colliderTriggerList;
				if (joinQuestColliderTriggerList.size() > 0)
				{
					if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Complete Collider Trigger Clear Condition");
					}
				}
			}

			// Clear - Collider Trigger 처리
			std::vector<QuestColliderTrigger>& clearQuestColliderTriggerList = mCurMainQuest.mclearConditionList.colliderTriggerList;
			if (clearQuestColliderTriggerList.size() > 0)
			{
				if (clearQuestColliderTriggerList[0].colliderName == event.colliderName)
				{
					if (!clearQuestColliderTriggerList[0].isAll)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurMainQuest, 0 });
						spdlog::trace("Complete Collider Trigger Single Clear Condition");
					}
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger>& clearQuestColliderTriggerList = mCurSubQuest[i].mclearConditionList.colliderTriggerList;
				if (clearQuestColliderTriggerList.size() > 0)
				{
					if (clearQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						if (!clearQuestColliderTriggerList[0].isAll)
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
			std::vector<Defence>& defenceList = mCurMainQuest.mclearConditionList.defenceList;
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
				std::vector<Defence>& defenceList = mCurSubQuest[i].mclearConditionList.defenceList;
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
			// 방금 깬 퀘스트가 클리어 조건으로 있는 퀘스트가 있다면 클리어 하기 
			std::vector<ClearQuest>& clearQuestList = mCurMainQuest.mclearConditionList.clearQuestList;
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
				std::vector<ClearQuest>& clearQuestList = mCurSubQuest[i].mclearConditionList.clearQuestList;
				if (clearQuestList.size() > 0)
				{
					if (clearQuestList[0].clearIndex == event.clearQuest.mIndex &&
						clearQuestList[0].clearIsMain == event.clearQuest.mIsMain)
					{
						spdlog::trace("Clear Quest Condition True!");
					}
				}
			}

			/// Quest Reward
			// 방금 깬 퀘스트에 포탈 보상이 있다면 보상 받기
			std::vector<RewardPortal> rewardPortalList = event.clearQuest.mRewardList.RewardPortalList;
			int rewardPortalListSize = rewardPortalList.size();
			if (rewardPortalListSize > 0)
			{
				for (int i = 0; i < rewardPortalListSize; i++)
				{
					// prefab으로 포탈 소환
					std::shared_ptr<game_module::GameObject> portal;

					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPortalPrefab);
					portal = *(instance.begin());

					// 트랜스폼 위치 바꾸기
					portal->GetComponent<game_module::Transform>()->SetLocalPosition(rewardPortalList[i].position);
					// Portal 컴포넌트에 접근해서 씬이름 바꾸기
					portal->GetComponent<Portal>()->SetNextSceneName(rewardPortalList[i].nextSceneName);

					GetScene()->AddGameObject(portal);
				}
			}

			// 방금 깬 퀘스트에 갑옷 보상이 있다면 보상 받기
			std::vector<ArmourSpawn> armourSpawnList = event.clearQuest.mRewardList.ArmourList;
			int armourSpawnListSize = armourSpawnList.size();
			if (armourSpawnListSize > 0)
			{
				for (int i = 0; i < armourSpawnListSize; i++)
				{
					std::vector<game_module::PrefabResource> armourPrefabList = 
						GetScene()->GetObjectByName(armourSpawnList[i].armourSetName)->GetComponent<ArmourSet>()->GetArmourPrefabList();

					unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
					std::mt19937 generator(seed);
					std::uniform_int_distribution<int> randomArmourDistribution(0, armourPrefabList.size() - 1);
					int randomArmour = randomArmourDistribution(generator);

					//for (int j = 0; j < armourPrefabList.size(); j++)
					{
						SpawnArmour(armourPrefabList[randomArmour]);
					}
				}
			}

			// 방금 깬 퀘스트에 시퀀스 시작이 있다면 시작
			std::vector<SequenceStart> sequenceStartList = event.clearQuest.mRewardList.SequenceStartList;
			int sequenceStartListSize = sequenceStartList.size();
			if (sequenceStartListSize > 0)
			{
				for (int i = 0; i < sequenceStartListSize; i++)
				{
					GetScene()->GetObjectByName(sequenceStartList[i].name)->GetComponent<game_module::Sequence>()->SetIsPlay(true);
				}
			}

			// 방금 깬 퀘스트가 선행퀘스트라면 다음 퀘스트 추가하기
			for (int i = 0; i < mMainQuests.size(); i++)
			{
				std::vector<PreQuest>& preQuestList = mMainQuests[i].mJoinConditionList.preQuestList;
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

			// 서브 퀘스트면 목록에서 삭제 
			if (!event.clearQuest.mIsMain)
			{
				mCurSubQuest.erase(mCurSubQuest.begin() + event.index);

				RenderOnSubQuest(mCurSubQuest.size(), false);
			}

			for (int i = 0; i < mSubQuests.size(); i++)
			{
				std::vector<PreQuest>& preQuestList = mSubQuests[i].mJoinConditionList.preQuestList;
				if (preQuestList.size() > 0)
				{
					if (preQuestList[0].preIndex == event.clearQuest.mIndex &&
						preQuestList[0].preIsMain == event.clearQuest.mIsMain)
					{
						mCurSubQuest.push_back(mSubQuests[i]);
						RenderOnSubQuest(i, true);
						spdlog::trace("Complete PreQuest!");
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
			std::vector<QuestColliderTrigger>& questColliderTriggerList = mCurMainQuest.mclearConditionList.colliderTriggerList;
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
				std::vector<QuestColliderTrigger>& questColliderTriggerList = mCurSubQuest[i].mclearConditionList.colliderTriggerList;
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
			std::vector<ObjectInteraction>& objectInteractionList = mCurMainQuest.mclearConditionList.objectInteration;
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
				std::vector<ObjectInteraction>& objectInteractionList = mCurSubQuest[i].mclearConditionList.objectInteration;
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

void fq::client::QuestManager::ViewQuestInformation(Quest quest, game_module::TextUI* textUI)
{
	// textUI children 0 - QuestBox, 1 - monsterKillText, 2 - GaugeBar
	game_module::TextUI* monsterKillText =
		textUI->GetGameObject()->GetParent()->GetChildren()[2]->GetComponent<game_module::TextUI>();
	
	auto text = monsterKillText->GetTextInfo();
	text.IsRender = false;
	monsterKillText->SetTextInfo(text);

	game_module::ImageUI* gaugeBar = textUI->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>();
	gaugeBar->SetIsRender(0, false);

	// Monster Kill Setting
	std::vector<MonsterKill>& monsterKillList = quest.mclearConditionList.monsterKillList;
	if (monsterKillList.size() > 0)
	{
		text.Text = std::to_string(monsterKillList[0].curNumber) + " / " + std::to_string(monsterKillList[0].requestsNumber);
		text.IsRender = true;
		monsterKillText->SetTextInfo(text);
	}

	// Monster Group Kill Setting
	std::vector<MonsterGroupKill>& monsterGroupKillList = quest.mclearConditionList.monsterGroupKillList;
	if (monsterGroupKillList.size() > 0)
	{
		text.Text = std::to_string(monsterGroupKillList[0].curNumber) + " / " + std::to_string(monsterGroupKillList[0].groupMonsterNumber);
		text.IsRender = true;
		monsterKillText->SetTextInfo(text);
	}

	// DefenceGauge Setting
	std::vector<Defence>& defence = quest.mclearConditionList.defenceList;
	if (defence.size() > 0)
	{
		float ratio = GetScene()->GetObjectByName(defence[0].colliderName)->GetComponent<DefenceCounter>()->GetCountRatio();
		auto imageInfo = gaugeBar->GetUIInfomation(0);
		imageInfo.Width = mGaugeMaxWidth * ratio;
		imageInfo.XRatio = ratio;
		gaugeBar->SetUIInfomation(0, imageInfo);

		gaugeBar->SetIsRender(0, true);
	}
}

void fq::client::QuestManager::RenderOnSubQuest(int i, bool isOn)
{
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren()[3]->GetChildren();

	auto subQuest = children[i]->GetChildren();

	auto text1 = subQuest[0]->GetComponent<game_module::TextUI>();
	auto image1 = subQuest[1]->GetComponent<game_module::ImageUI>();
	auto text2 = subQuest[2]->GetComponent<game_module::TextUI>();
	auto image2 = subQuest[3]->GetComponent<game_module::ImageUI>();

	auto textInfo = text1->GetTextInfo();
	textInfo.IsRender = isOn;
	text1->SetTextInfo(textInfo);

	textInfo = text2->GetTextInfo();
	textInfo.IsRender = isOn;
	text2->SetTextInfo(textInfo);

	image1->SetIsRender(0, isOn);
	image2->SetIsRender(0, isOn);
}

void fq::client::QuestManager::SetScaleAndPositionScreen()
{
	// Scale 자동 조정 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}

	// Text 크기 및 위치 변경 (미적용 중)
	float xOffset = -6.f;
	float yOffset = -2.6f;

	auto textInfo = mMainQuestText->GetTextInfo();
	textInfo.FontSize = mFontSize * myTransform->GetWorldScale().x;
	//mMainQuestText->SetTextInfo(textInfo);
	int deltaFontSize = textInfo.FontSize - mFontSize;
	//mMainQuestText->GetTransform()->SetLocalPosition({ (float)deltaFontSize * xOffset, (float)deltaFontSize * -yOffset, 0});

	for (int i = 0; i < mSubQuestTexts.size(); i++)
	{
		textInfo = mSubQuestTexts[i]->GetTextInfo();
		textInfo.FontSize = mFontSize * myTransform->GetWorldScale().x;
		//mSubQuestTexts[i]->SetTextInfo(textInfo);
		//mSubQuestTexts[i]->GetTransform()->SetLocalPosition({ (float)deltaFontSize * xOffset, (float)deltaFontSize * -yOffset, 0 });
	}

	game_module::ImageUI* myImage = GetComponent<game_module::ImageUI>();
	// Position 자동 조정
	{
		myTransform->SetLocalPosition({ 0.85f * screenWidth, 0.45f * screenHeight, 1 });
	}
}

void fq::client::QuestManager::SpawnArmour(fq::game_module::PrefabResource armour)
{
	// 갑옷 소환
	std::shared_ptr<game_module::GameObject> armourObject;

	//for (int aaa = 0; aaa < 1000; aaa++) 디버깅용
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(armour);
		armourObject = *(instance.begin());

		GetScene()->AddGameObject(armourObject);

		/// 갑옷 위치 설정
		// 카메라 중심 가져오기
		DirectX::SimpleMath::Vector3 center = GetScene()->GetObjectByName("MainCamera")->GetComponent<CameraMoving>()->GetCenterCameraInWorld();
		DirectX::SimpleMath::Vector3 nearPos = { 0, 0, 0 };

		// 랜덤 위치 설정
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distanceDistribution(0, mDistance);
		std::uniform_real_distribution<float> degreeDistribution(0, 360);

		float randomDistance = distanceDistribution(generator);
		float randomDegree = degreeDistribution(generator);

		float radian = randomDegree * 3.1415926535f / 180.0f;

		center.x += std::cosf(radian) * randomDistance;
		center.z += std::sinf(radian) * randomDistance;

		// 유효한 위치인지 확인
		int count = 0;
		bool isValid = false;
		while (!isValid && count < 100)
		{
			isValid = GetGameObject()->GetComponent<game_module::NavigationAgent>()->IsValid(center, nearPos);
			count++;
		}
		nearPos.y += 1.0f;
		armourObject->GetComponent<game_module::Transform>()->SetLocalPosition(nearPos);
	}
}

