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
#include "LevelHepler.h"
#include "QuestColliderTriggerChecker.h"
#include "PlayerInfoVariable.h"

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
	mArmourSpawnDistance(5)
{
}

fq::client::QuestManager::QuestManager(const QuestManager& other)
	:mStartQuests(other.mStartQuests),
	mMainQuests(other.mMainQuests),
	mSubQuests(other.mSubQuests),
	mCurMainQuest(other.mCurMainQuest),
	mCurSubQuest(other.mCurSubQuest),
	mPortalPrefab(other.mPortalPrefab),
	mArmourSpawnDistance(other.mArmourSpawnDistance)
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
	mArmourSpawnDistance = other.mArmourSpawnDistance;

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
	initSTL(); // STL 초기화

	setMonsterGroup(); // Quest 마다 monster Group Max 값 설정
	setColliderTriggerChecker(); // QuestColliderTrigger 마다 questManager에 설정된 값 설정

	// 이벤트 핸들러 등록
	eventProcessKillMonster();
	eventProcessPlayerCollideTrigger();
	eventProcessCompleteDefence();
	eventProcessClearQuest();
	eventProcessAllColliderTrigger();
	eventProcessObjectInteraction();
	eventProcessClearGoddessStatue();
	eventProcessChangePlayerNumCollideTrigger();
	eventProcessInProgressDefence();
	eventProcessPushButtonEvent();
	eventProcessOnUIRender();

	mIsRenderingUI = true;

	// Quest 마다 isMain 설정
	for (int i = 0; i < mMainQuests.size(); i++)
	{
		mMainQuests[i].mIsMain = true;
	}
	for (int i = 0; i < mSubQuests.size(); i++)
	{
		mSubQuests[i].mIsMain = false;
	}

	// 시작 퀘스트 등록
	auto it = std::find_if(mMainQuests.begin(), mMainQuests.end(), [this](Quest quest)
		{
			return quest.mIndex == mStartQuests.startMainQuestIndex;
		});
	mCurMainQuest = *it;
	mViewMainQuest = mCurMainQuest;

	for (int i = 0; i < mStartQuests.startSubQuestIndex.size(); i++)
	{
		auto it = std::find_if(mSubQuests.begin(), mSubQuests.end(), [this, i](Quest quest)
			{
				return quest.mIndex == mStartQuests.startSubQuestIndex[i].index;
			});
		mCurSubQuest.push_back(*it);
	}
	mViewSubQuest = mCurSubQuest;

	// 오브젝트 설정
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren();
	mMainQuestText = children[1]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	for (int i = 0; i < 3; i++)
	{
		mSubQuestTexts.push_back(children[3]->GetChildren()[i]->GetChildren()[0]->GetComponent<game_module::TextUI>());
	}

	/// Scale 설정
	mScreenManager = GetScene()->GetScreenManager();
	// GaugeMaxWidth 설정
	mGaugeMaxWidth = mMainQuestText->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>()->GetUIInfomation(0).Width;
	// 기본 FontSize 설정
	mFontSize = mMainQuestText->GetTextInfo().FontSize;

	/// 연출용 UI
	mNewImages.push_back(children[1]->GetChildren()[4]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	mCompleteImages.push_back(children[1]->GetChildren()[4]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	mQuestBoxes.push_back(children[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	for (int i = 0; i < 3; i++)
	{
		mNewImages.push_back(children[3]->GetChildren()[i]->GetChildren()[4]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mCompleteImages.push_back(children[3]->GetChildren()[i]->GetChildren()[4]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
		mFailImages.push_back(children[3]->GetChildren()[i]->GetChildren()[4]->GetChildren()[2]->GetComponent<game_module::ImageUI>());
		mFailImages[i]->SetIsRender(0, false);
		mFailImageCounts.push_back(0);
		mIsFinishedFailedAnimation.push_back(0);
		mIsInProgressDefence.push_back(false);
		mIsInProgressDefenceView.push_back(false);
		mQuestBoxes.push_back(children[3]->GetChildren()[i]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	}
	for (int i = 0; i < 4; i++)
	{
		mNewImageCounts.push_back(0);
		mCompleteImageCounts.push_back(0);
		mNewImages[i]->SetIsRender(0, false);
		mCompleteImages[i]->SetIsRender(0, false);
		mIsFinishedCompleteAnimation.push_back(false);
	}

	mLeftChecks.push_back(children[1]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	mRightChecks.push_back(children[1]->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());

	for (int i = 0; i < 3; i++)
	{
		mLeftChecks.push_back(children[3]->GetChildren()[i]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mRightChecks.push_back(children[3]->GetChildren()[i]->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	}

	// 시작 서브퀘스트 개수 만큼만 QuestUI Render 하기 
	RenderOnAllSubQuest();

	// 버튼 퀘스트 
	for (int i = 0; i < 4; i++)
	{
		mIsAlive.push_back(false);
		mIsUseX.push_back(false);
		mIsUseY.push_back(false);
		mIsUseR.push_back(false);
		mIsUseA.push_back(false);
	}

	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		mIsAlive[0] = true;
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		mIsAlive[1] = true;
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		mIsAlive[2] = true;
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		mIsAlive[3] = true;
	}

	while (!mRemoveViewQuests.empty())
	{
		mRemoveViewQuests.pop();
	}
}

void fq::client::QuestManager::OnUpdate(float dt)
{
	if (mClearEvents.size() > 0)
	{
		for (int i = 0; i < mClearEvents.size(); i++)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
				{ mClearEvents[i], mClearEventIndexes[i] });
		}
		mClearEvents.clear();
		mClearEventIndexes.clear();
	}

	setScaleAndPositionScreen();

	// Main Quest Text Setting
	auto textInfo = mMainQuestText->GetTextInfo();
	textInfo.Text = mViewMainQuest.mName;
	mMainQuestText->SetTextInfo(textInfo);

	ViewQuestInformation(mViewMainQuest, mMainQuestText);

	// Sub Quest Text Setting
	for (int i = 0; i < mViewSubQuest.size(); i++)
	{
		auto textInfo = mSubQuestTexts[i]->GetTextInfo();
		textInfo.Text = mViewSubQuest[i].mName;
		mSubQuestTexts[i]->SetTextInfo(textInfo);
		ViewQuestInformation(mViewSubQuest[i], mSubQuestTexts[i]);
		if (!mIsInProgressDefenceView[i])
		{
			playTimer(i, mViewSubQuest[i], dt, true);
		}
	}

	for (int i = 0; i < mCurSubQuest.size(); i++)
	{
		if (!mIsInProgressDefence[i])
		{
			playTimer(i, mCurSubQuest[i], dt, false);
		}
	}

	GetScene()->GetEventManager()->FireEvent<client::event::CurrentQuest>({ true, mCurMainQuest.mIndex });
	for (int i = 0; i < mCurSubQuest.size(); i++)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::CurrentQuest>({ false, mCurSubQuest[i].mIndex });
	}

	playNew(dt);
	playComplete(dt);
	PlayFail(dt);

	for (int i = 0; i < 3; i++)
	{
		mIsInProgressDefence[i] = false;
		mIsInProgressDefenceView[i] = false;
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
	GetScene()->GetEventManager()->RemoveHandle(mClearGoddessStatueHandler);
	GetScene()->GetEventManager()->RemoveHandle(mChangePlayerNumCollideTriggereHandler);
	GetScene()->GetEventManager()->RemoveHandle(mInProgressDefenceHandler);
	GetScene()->GetEventManager()->RemoveHandle(mPushButtonHandler);
	GetScene()->GetEventManager()->RemoveHandle(mUIRenderHandler);
}

void fq::client::QuestManager::eventProcessKillMonster()
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
						spdlog::trace("curNumber: {}, requestNumber: {}", monsterKillList[i].curNumber, monsterKillList[i].requestsNumber * LevelHepler::GetSpawnRatio());

						if (monsterKillList[i].curNumber == monsterKillList[i].requestsNumber * LevelHepler::GetSpawnRatio())
						{
							monsterKillList[i].isClear = true;
						}
					}
					else if (monsterKillList[i].monsterType == EMonsterType::All)
					{
						monsterKillList[i].curNumber++;
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
							spdlog::trace("curNumber: {}, requestNumber: {}", monsterKillList[i].curNumber, monsterKillList[i].requestsNumber * LevelHepler::GetSpawnRatio());

							if (monsterKillList[i].curNumber == monsterKillList[i].requestsNumber * LevelHepler::GetSpawnRatio())
							{
								monsterKillList[i].isClear = true;
							}

							for (int i = 0; i < mViewSubQuest.size(); i++)
							{
								if (mViewSubQuest[i].mIndex == mCurSubQuest[j].mIndex)
								{
									mViewSubQuest[i] = mCurSubQuest[j];
								}
							}
						}
						else if (monsterKillList[i].monsterType == EMonsterType::All)
						{
							monsterKillList[i].curNumber++;

							if (mViewSubQuest[i].mIndex == mCurSubQuest[j].mIndex)
							{
								mViewSubQuest[i] = mCurSubQuest[j];
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
							{ mCurSubQuest[j], j });
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

				if (mCurMainQuest.mIndex == mViewMainQuest.mIndex)
				{
					mViewMainQuest.mclearConditionList.monsterGroupKillList[0].groupMonsterNumber = monsterGroupKillList[0].groupMonsterNumber;
					mViewMainQuest.mclearConditionList.monsterGroupKillList[0].curNumber = monsterGroupKillList[0].curNumber;
				}

				if (monsterGroupKillList[0].curNumber == 0)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("MonsterGroupKill Quest Clear!");
				}
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
					spdlog::trace("total {}, cur {}", monsterGroup->GetAllMonsterSize(), monsterGroup->GetRemainMonsterSize());

					for (int j = 0; j < mViewSubQuest.size(); j++)
					{
						if (mViewSubQuest[j].mIndex == mCurSubQuest[i].mIndex)
						{
							mViewSubQuest[j].mclearConditionList.monsterGroupKillList[0].groupMonsterNumber = monsterGroupKillList[0].groupMonsterNumber;
							mViewSubQuest[j].mclearConditionList.monsterGroupKillList[0].curNumber = monsterGroupKillList[0].curNumber;
						}
					}

					if (monsterGroupKillList[0].curNumber == 0)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("MonsterGroupKill Quest Clear!");
					}
				}

			}
		});
}

void fq::client::QuestManager::eventProcessPlayerCollideTrigger()
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
			for (int i = 0; i < mMainQuests.size(); i++)
			{
				std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mMainQuests[i].mJoinConditionList.colliderTriggerList;

				if (joinQuestColliderTriggerList.size() > 0)
				{
					if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						if (!joinQuestColliderTriggerList[0].isAll && joinQuestColliderTriggerList[0].playerNumber == -1)
						{
							mCurMainQuest = mMainQuests[i];

							spdlog::trace("Complete Collider Trigger Join Condition");
						}
					}
				}
			}

			for (int i = 0; i < mSubQuests.size(); i++)
			{
				std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mSubQuests[i].mJoinConditionList.colliderTriggerList;
				if (joinQuestColliderTriggerList.size() > 0)
				{
					if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						if (!joinQuestColliderTriggerList[0].isAll && joinQuestColliderTriggerList[0].playerNumber == -1)
						{
							// isClear가 아니면 // 현재 목록에 없다면 // 렌더도 해야함 // 지금 서브 퀘스트가 3개보다 작은지 체크도 해야함 
							bool isInProgress = false;
							for (int j = 0; j < mCurSubQuest.size(); j++)
							{
								if (mCurSubQuest[j].mIndex == mSubQuests[i].mIndex)
								{
									isInProgress = true;
								}
							}

							if ((mCurSubQuest.size() < 3 && !mSubQuests[i].mIsClear) && !isInProgress)
							{
								mCurSubQuest.push_back(mSubQuests[i]);
								mViewSubQuest.push_back(mSubQuests[i]);

								RenderOnAllSubQuest();

								// New 연출
								int newSubQuestIndex = mViewSubQuest.size();
								startNew(newSubQuestIndex);

								spdlog::trace("Complete Collider Trigger Join Condition");
							}
						}
					}
				}
			}

			// Clear - Collider Trigger 처리
			std::vector<QuestColliderTrigger>& clearQuestColliderTriggerList = mCurMainQuest.mclearConditionList.colliderTriggerList;
			if (clearQuestColliderTriggerList.size() > 0)
			{
				if (clearQuestColliderTriggerList[0].colliderName == event.colliderName)
				{
					if (!clearQuestColliderTriggerList[0].isAll && clearQuestColliderTriggerList[0].playerNumber == -1)
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
						if (!clearQuestColliderTriggerList[0].isAll && clearQuestColliderTriggerList[0].playerNumber == -1)
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

void fq::client::QuestManager::eventProcessCompleteDefence()
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

void fq::client::QuestManager::eventProcessClearQuest()
{
	mClearQuestHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ClearQuestEvent>(
		[this](const client::event::ClearQuestEvent& event)
		{
			initPushPlayer(mIsUseX);
			initPushPlayer(mIsUseY);
			initPushPlayer(mIsUseR);
			initPushPlayer(mIsUseA);

			if (event.clearQuest.mIsMain)
			{
				for (int i = 0; i < mMainQuests.size(); i++)
				{
					if (event.clearQuest.mIndex == mMainQuests[i].mIndex)
					{
						mMainQuests[i].mIsClear = true;
					}
				}
				if (event.clearQuest.mIndex == mCurMainQuest.mIndex)
				{
					mCurMainQuest.mIsClear = true;
				}
				if (event.clearQuest.mIndex == mViewMainQuest.mIndex)
				{
					mViewMainQuest.mIsClear = true;
				}
			}
			else
			{
				for (int i = 0; i < mSubQuests.size(); i++)
				{
					if (event.clearQuest.mIndex == mSubQuests[i].mIndex)
					{
						mSubQuests[i].mIsClear = true;
					}
				}
			}

			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Quest_complete", false , fq::sound::EChannel::SE });

			// 방금 깬 퀘스트가 클리어 조건으로 있는 퀘스트가 있다면 클리어 하기 
			std::vector<ClearQuest>& clearQuestList = mCurMainQuest.mclearConditionList.clearQuestList;
			if (clearQuestList.size() > 0)
			{
				bool isClear = true;
				for (int i = 0; i < clearQuestList.size(); i++)
				{
					if (clearQuestList[i].clearIndex == event.clearQuest.mIndex &&
						clearQuestList[i].clearIsMain == event.clearQuest.mIsMain)
					{
						clearQuestList[i].isClear = true;

						spdlog::trace("Clear Quest Condition True!");
					}

					if (!clearQuestList[i].isClear)
					{
						isClear = false;
					}
				}
				if (isClear)
				{
					mClearEvents.push_back(mCurMainQuest);
					mClearEventIndexes.push_back(0);
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<ClearQuest>& clearQuestList = mCurSubQuest[i].mclearConditionList.clearQuestList;
				if (clearQuestList.size() > 0)
				{
					bool isClear = true;
					for (int j = 0; j < clearQuestList.size(); j++)
					{
						if (clearQuestList[j].clearIndex == event.clearQuest.mIndex &&
							clearQuestList[j].clearIsMain == event.clearQuest.mIsMain)
						{
							clearQuestList[j].isClear = true;

							spdlog::trace("Clear Quest Condition True!");
						}
						if (!clearQuestList[j].isClear)
						{
							isClear = false;
						}
					}
					if (isClear)
					{
						mClearEvents.push_back(mCurSubQuest[i]);
						mClearEventIndexes.push_back(i);
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
					auto sequenceObject = GetScene()->GetObjectByName(sequenceStartList[i].name);
					if (sequenceObject)
					{
						sequenceObject->GetComponent<game_module::Sequence>()->SetIsPlay(true);
					}
					else
					{
						spdlog::warn("[QuestManager] {} object does not exist", sequenceStartList[i].name);
					}
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
						startNew(0);
						spdlog::trace("Complete PreQuest!");
					}
				}
			}

			// 서브 퀘스트면 목록에서 삭제 
			if (!event.clearQuest.mIsMain)
			{
				mCurSubQuest.erase(mCurSubQuest.begin() + event.index);

				// Complete 연출
				if (!event.clearQuest.mIsMain)
				{
					for (int i = 0; i < mViewSubQuest.size(); i++)
					{
						if (mViewSubQuest[i].mIndex == event.clearQuest.mIndex)
						{
							startComplete(i + 1);
							mRemoveViewQuests.push(mViewSubQuest[i]);
						}
					}
				}
			}
			else
			{
				// Complete 연출
				int questUIindex = 0;
				startComplete(questUIindex);
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
						mNextSubQuests.push_back(mSubQuests[i]);
						spdlog::trace("Complete PreQuest!");
					}
				}
			}

			for (std::list<Quest>::iterator it = mNextSubQuests.begin(); it != mNextSubQuests.end(); ++it)
			{
				if (it->mIndex == event.clearQuest.mIndex &&
					it->mIsMain == event.clearQuest.mIsMain)
				{
					mNextSubQuests.erase(it);
				}
			}
		});
}

void fq::client::QuestManager::eventProcessAllColliderTrigger()
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

			// Join
			for (int i = 0; i < mMainQuests.size(); i++)
			{
				std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mMainQuests[i].mJoinConditionList.colliderTriggerList;

				if (joinQuestColliderTriggerList.size() > 0)
				{
					if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						mCurMainQuest = mMainQuests[i];

						spdlog::trace("Complete Collider Trigger Join Condition");
					}
				}
			}

			for (int i = 0; i < mSubQuests.size(); i++)
			{
				std::vector<QuestColliderTrigger>& joinQuestColliderTriggerList = mSubQuests[i].mJoinConditionList.colliderTriggerList;
				if (joinQuestColliderTriggerList.size() > 0)
				{
					if (joinQuestColliderTriggerList[0].colliderName == event.colliderName)
					{
						// isClear가 아니면 // 현재 목록에 없다면 // 렌더도 해야함 // 지금 서브 퀘스트가 3개보다 작은지 체크도 해야함 
						bool isInProgress = false;
						for (int j = 0; j < mCurSubQuest.size(); j++)
						{
							if (mCurSubQuest[j].mIndex == mSubQuests[i].mIndex)
							{
								isInProgress = true;
							}
						}

						if ((mCurSubQuest.size() < 3 && !mSubQuests[i].mIsClear) && !isInProgress)
						{
							mCurSubQuest.push_back(mSubQuests[i]);
							mViewSubQuest.push_back(mSubQuests[i]);

							RenderOnAllSubQuest();

							// New 연출
							int newSubQuestIndex = mViewSubQuest.size();
							startNew(newSubQuestIndex);

							spdlog::trace("Complete Collider Trigger Join Condition");
						}
					}
				}
			}
		});
}

void fq::client::QuestManager::eventProcessObjectInteraction()
{
	mObjectInteractionHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ObjectInteractionEvent>(
		[this](const client::event::ObjectInteractionEvent& event)
		{
			std::vector<ObjectInteraction>& objectInteractionList = mCurMainQuest.mclearConditionList.objectInterationList;
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
				std::vector<ObjectInteraction>& objectInteractionList = mCurSubQuest[i].mclearConditionList.objectInterationList;
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
	if (!mIsRenderingUI)
	{
		return;
	}

	// textUI children 0 - QuestBox, 1 - monsterKillText, 2 - GaugeBar
	game_module::TextUI* monsterKillText =
		textUI->GetGameObject()->GetParent()->GetChildren()[2]->GetComponent<game_module::TextUI>();

	game_module::TextUI* timerText =
		textUI->GetGameObject()->GetParent()->GetChildren()[5]->GetComponent<game_module::TextUI>();

	auto text = monsterKillText->GetTextInfo();
	text.IsRender = false;
	monsterKillText->SetTextInfo(text);

	timerText->SetIsRender(false);

	game_module::ImageUI* gaugeBar = textUI->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>();
	gaugeBar->SetIsRender(0, false);

	game_module::ImageUI* gaugeBarBack = nullptr;
	if (gaugeBar->GetGameObject()->GetChildren().size() > 0)
	{
		gaugeBarBack = gaugeBar->GetGameObject()->GetChildren()[0]->GetComponent<game_module::ImageUI>();
		gaugeBarBack->SetIsRender(0, false);
	}

	// Monster Kill Setting
	std::vector<MonsterKill>& monsterKillList = quest.mclearConditionList.monsterKillList;
	if (monsterKillList.size() > 0)
	{
		text.Text = std::to_string(monsterKillList[0].curNumber) + " / " + std::to_string(int(monsterKillList[0].requestsNumber * LevelHepler::GetSpawnRatio()));
		text.IsRender = true;
		monsterKillText->SetTextInfo(text);
	}

	// Collider Trigger Setting
	std::vector<QuestColliderTrigger>& colliderTriggerList = quest.mclearConditionList.colliderTriggerList;
	if (colliderTriggerList.size() > 0)
	{
		if (colliderTriggerList[0].isAll || colliderTriggerList[0].playerNumber != -1)
		{
			if (!quest.mIsClear)
			{
				text.Text = std::to_string(colliderTriggerList[0].curPlayer) + " / " + std::to_string(colliderTriggerList[0].maxPlayer);
				text.IsRender = true;
				monsterKillText->SetTextInfo(text);
			}
		}
	}

	// Monster Group Kill Setting
	std::vector<MonsterGroupKill>& monsterGroupKillList = quest.mclearConditionList.monsterGroupKillList;
	if (monsterGroupKillList.size() > 0)
	{
		text.Text = std::to_string(monsterGroupKillList[0].groupMonsterNumber - monsterGroupKillList[0].curNumber) + " / " + std::to_string(monsterGroupKillList[0].groupMonsterNumber);
		//spdlog::trace("cur {} / max {}", monsterGroupKillList[0].curNumber, monsterGroupKillList[0].groupMonsterNumber);
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
		if (gaugeBarBack != nullptr)
		{
			gaugeBarBack->SetIsRender(0, true);
		}
	}

	// PushButton
	std::vector<PushButton>& pushButtonList = quest.mclearConditionList.pushButtonList;
	if (pushButtonList.size() > 0)
	{
		int pushButtonPlayer = -1;
		switch (pushButtonList[0].skillType)
		{
			case ESkillType::X:
				pushButtonPlayer = getXPushPlayer();
				break;
			case ESkillType::A:
				pushButtonPlayer = getAPushPlayer();
				break;
			case ESkillType::R:
				pushButtonPlayer = getRPushPlayer();
				break;
			case ESkillType::Y:
				pushButtonPlayer = getYPushPlayer();
				break;
			default:
				break;
		}
		text.Text = std::to_string(pushButtonPlayer) + " / " + std::to_string(getMaxPlayer());
		text.IsRender = true;
		monsterKillText->SetTextInfo(text);
	}

	if (quest.mIsTime && !quest.mIsClear)
	{
		timerText->SetIsRender(true);
		std::string sec = std::to_string(((int)quest.mSeconds + 1) / 60) + ":" + std::to_string(((int)quest.mSeconds + 1) % 60);
		timerText->SetText(sec);
		if (int(quest.mSeconds) <= 10)
		{
			auto textInfo = timerText->GetTextInfo();
			textInfo.FontColor = { 1, 0, 0, 1 };
			timerText->SetTextInfo(textInfo);
		}
		else
		{
			auto textInfo = timerText->GetTextInfo();
			textInfo.FontColor = { 1, 1, 1, 1 };
			timerText->SetTextInfo(textInfo);
		}
	}
}

void fq::client::QuestManager::RenderOnSubQuest(int i, bool isOn)
{
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren()[3]->GetChildren();

	auto subQuest = children[i]->GetChildren();

	auto text1 = subQuest[0]->GetComponent<game_module::TextUI>();
	auto image1 = subQuest[1]->GetComponent<game_module::ImageUI>();

	auto textInfo = text1->GetTextInfo();
	textInfo.IsRender = isOn;
	text1->SetTextInfo(textInfo);
	if (!isOn)
	{
		auto text2 = subQuest[2]->GetComponent<game_module::TextUI>();
		auto image3 = subQuest[3]->GetComponent<game_module::ImageUI>();
		auto text5 = subQuest[5]->GetComponent<game_module::TextUI>();
		auto image3child = subQuest[3]->GetChildren()[0]->GetComponent<game_module::ImageUI>();

		text2->SetIsRender(isOn);
		text5->SetIsRender(isOn);

		image1->SetIsRender(0, isOn);
		image3->SetIsRender(0, isOn);
		image3child->SetIsRender(0, isOn);
	}

	auto uiInfo = mQuestBoxes[i + 1]->GetUIInfomation(0);
	uiInfo.Alpha = 1;
	uiInfo.isRender = isOn;
	mQuestBoxes[i + 1]->SetUIInfomation(0, uiInfo);
}

void fq::client::QuestManager::setScaleAndPositionScreen()
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
		std::uniform_real_distribution<float> distanceDistribution(0, mArmourSpawnDistance);
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
			if (auto agent = GetComponent<game_module::NavigationAgent>(); agent != nullptr)
			{
				agent->IsValid(center, nearPos);
			}
			else
			{
				spdlog::warn("[QusetManager] {} Has not NavigationAgent", GetGameObject()->GetName());
				return;
			}
			count++;
		}
		nearPos.y += 1.2f;
		armourObject->GetComponent<game_module::Transform>()->SetLocalPosition(nearPos);
	}
}

void fq::client::QuestManager::eventProcessClearGoddessStatue()
{
	mClearGoddessStatueHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ClearGoddessStatue>(
		[this](const client::event::ClearGoddessStatue& event)
		{
			std::vector<ClearGoddessStatue>& clearGoddessStatueList = mCurMainQuest.mclearConditionList.clearGoddessStatueList;
			if (clearGoddessStatueList.size() > 0)
			{
				if (clearGoddessStatueList[0].goddessStatueName == event.goddessStatueName)
				{
					GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						{ mCurMainQuest, 0 });
					spdlog::trace("Complete Clear Goddess Statue");
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<ClearGoddessStatue>& clearGoddessStatueList = mCurSubQuest[i].mclearConditionList.clearGoddessStatueList;
				if (clearGoddessStatueList.size() > 0)
				{
					if (clearGoddessStatueList[0].goddessStatueName == event.goddessStatueName)
					{
						GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							{ mCurSubQuest[i], i });
						spdlog::trace("Complete Clear Goddess Statue");
					}
				}
			}
		});
}

void fq::client::QuestManager::playNew(float dt)
{
	for (int i = 0; i < 4; i++)
	{
		if (i == 0 && mCompleteImageCounts[0] > 0)
		{
			continue;
		}
		if (mNewImageCounts[i] > 0)
		{
			auto newInfo = mNewImages[i]->GetUIInfomation(0);
			newInfo.isRender = true;
			newInfo.Alpha = 1;
			mNewImages[i]->SetUIInfomation(0, newInfo);
			if (mNewImageCounts[i] < 1.0f)
			{
				auto uiInfo = mNewImages[i]->GetUIInfomation(0);
				uiInfo.Alpha = mNewImageCounts[i];
				mNewImages[i]->SetUIInfomation(0, uiInfo);
			}
			mNewImageCounts[i] -= dt;
		}
		else
		{
			mNewImages[i]->SetIsRender(0, false);
		}
	}
}

void fq::client::QuestManager::playComplete(float dt)
{
	// 체크 둘다 생기고 0.5 왼쪽 작아짐 0.5 오른쪽 작아짐 1초 후 페이드 아웃
	for (int i = 0; i < 4; i++)
	{
		if (mCompleteImageCounts[i] > 0)
		{
			game_module::TextUI* textUI;
			if (i == 0)
			{
				textUI = mMainQuestText;
			}
			else
			{
				textUI = mSubQuestTexts[i - 1];
			}
			// Gauge Bar, 몬스터 Text 지우기
			game_module::ImageUI* gaugeBar = textUI->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>();
			gaugeBar->SetIsRender(0, false);

			game_module::ImageUI* gaugeBarBack = nullptr;
			if (gaugeBar->GetGameObject()->GetChildren().size() > 0)
			{
				gaugeBarBack = gaugeBar->GetGameObject()->GetChildren()[0]->GetComponent<game_module::ImageUI>();
				gaugeBarBack->SetIsRender(0, false);
			}

			game_module::TextUI* monsterKillText =
				textUI->GetGameObject()->GetParent()->GetChildren()[2]->GetComponent<game_module::TextUI>();
			monsterKillText->SetIsRender(false);

			game_module::TextUI* timerText =
				textUI->GetGameObject()->GetParent()->GetChildren()[5]->GetComponent<game_module::TextUI>();
			timerText->SetIsRender(false);

			if (mCompleteImageCounts[i] > 2.5f)
			{
				auto uiInfo = mLeftChecks[i]->GetUIInfomation(0);
				uiInfo.Width = ((mCompleteImageCounts[i] - 2.5f) / 0.5f) * 70 + 30;
				uiInfo.Height = ((mCompleteImageCounts[i] - 2.5f) / 0.5f) * 70 + 30;
				mLeftChecks[i]->SetUIInfomation(0, uiInfo);
			}
			else if (mCompleteImageCounts[i] > 2.0f)
			{
				auto uiInfo = mRightChecks[i]->GetUIInfomation(0);
				uiInfo.Width = ((mCompleteImageCounts[i] - 2.0f) / 0.5f) * 70 + 30;
				uiInfo.Height = ((mCompleteImageCounts[i] - 2.0f) / 0.5f) * 70 + 30;
				mRightChecks[i]->SetUIInfomation(0, uiInfo);
			}

			// 페이드 아웃 처리 
			else if (mCompleteImageCounts[i] < 1.0f)
			{
				auto uiInfo = mCompleteImages[i]->GetUIInfomation(0);
				uiInfo.Alpha = mCompleteImageCounts[i];
				mCompleteImages[i]->SetUIInfomation(0, uiInfo);

				uiInfo = mLeftChecks[i]->GetUIInfomation(0);
				uiInfo.Alpha = mCompleteImageCounts[i];
				mLeftChecks[i]->SetUIInfomation(0, uiInfo);

				uiInfo = mRightChecks[i]->GetUIInfomation(0);
				uiInfo.Alpha = mCompleteImageCounts[i];
				mRightChecks[i]->SetUIInfomation(0, uiInfo);

				if (i == 0)
				{
					if (mNewImageCounts[0] > 0)
					{
						auto textInfo = mMainQuestText->GetTextInfo();
						textInfo.FontColor.A(mCompleteImageCounts[i]);
						mMainQuestText->SetTextInfoPlay(textInfo);

						auto questBox = textUI->GetGameObject()->GetParent()->GetChildren()[1]->GetComponent<game_module::ImageUI>();
						uiInfo = questBox->GetUIInfomation(0);
						uiInfo.Alpha = mCompleteImageCounts[i];
						questBox->SetUIInfomation(0, uiInfo);
					}
				}
				else
				{
					auto textInfo = mSubQuestTexts[i - 1]->GetTextInfo();
					textInfo.FontColor.A(mCompleteImageCounts[i]);
					mSubQuestTexts[i - 1]->SetTextInfoPlay(textInfo);

					auto questBox = textUI->GetGameObject()->GetParent()->GetChildren()[1]->GetComponent<game_module::ImageUI>();
					uiInfo = questBox->GetUIInfomation(0);
					uiInfo.Alpha = mCompleteImageCounts[i];
					questBox->SetUIInfomation(0, uiInfo);
				}
			}

			mCompleteImageCounts[i] -= dt;
			mIsFinishedCompleteAnimation[i] = true;
		}
		else
		{
			mCompleteImages[i]->SetIsRender(0, false);
			mLeftChecks[i]->SetIsRender(0, false);
			mRightChecks[i]->SetIsRender(0, false);

			// Main
			if (i == 0)
			{
				auto textInfo = mMainQuestText->GetTextInfo();
				textInfo.FontColor.A(1);
				mMainQuestText->SetTextInfoPlay(textInfo);

				mViewMainQuest = mCurMainQuest;

				auto questBox = GetGameObject()->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>();
				auto uiInfo = questBox->GetUIInfomation(0);
				uiInfo.Alpha = 1;
				questBox->SetUIInfomation(0, uiInfo);

				if (mIsFinishedCompleteAnimation[i])
				{
					mIsFinishedCompleteAnimation[i] = false;

					int nextSubQuestSize = mNextSubQuests.size() + 1;
					for (int j = 1; j < nextSubQuestSize; j++)
					{
						// 다음 퀘스트 추가
						mViewSubQuest.push_back(mNextSubQuests.front());
						mNextSubQuests.pop_front();

						startNew(j);
					}
					RenderOnAllSubQuest();
				}
			}
			// Sub
			else
			{
				auto textInfo = mSubQuestTexts[i - 1]->GetTextInfo();
				textInfo.FontColor.A(1);
				mSubQuestTexts[i - 1]->SetTextInfoPlay(textInfo);

				if (mIsFinishedCompleteAnimation[i])
				{
					mIsFinishedCompleteAnimation[i] = false;
					//if (mViewSubQuest.size() > 0)
					//{
					//	mViewSubQuest.erase(mViewSubQuest.begin() + i - 1);
					//}

					Quest removeQuest = mRemoveViewQuests.front();
					mRemoveViewQuests.pop();

					for (int viewQuest = 0; viewQuest < mViewSubQuest.size(); viewQuest++)
					{
						if (mViewSubQuest[viewQuest].mIndex == removeQuest.mIndex)
						{
							mViewSubQuest.erase(mViewSubQuest.begin() + viewQuest);
						}
					}

					if (mNextSubQuests.size() > 0)
					{
						// 다음 퀘스트 추가
						mViewSubQuest.push_back(mNextSubQuests.front());
						mNextSubQuests.pop_front();

						// New 연출
						startNew(mViewSubQuest.size());
					}

					RenderOnAllSubQuest();
				}
			}
		}
	}
}

void fq::client::QuestManager::eventProcessChangePlayerNumCollideTrigger()
{
	mChangePlayerNumCollideTriggereHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::ChangePlayerNumCollideTrigger>(
		[this](const client::event::ChangePlayerNumCollideTrigger& event)
		{
			std::vector<QuestColliderTrigger>& colliderTriggerList = mViewMainQuest.mclearConditionList.colliderTriggerList;
			if (colliderTriggerList.size() > 0)
			{
				if (colliderTriggerList[0].colliderName == event.colliderName)
				{
					colliderTriggerList[0].maxPlayer = event.maxPlayer;
					colliderTriggerList[0].curPlayer = event.curPlayer;
				}
			}

			for (int i = 0; i < mViewSubQuest.size(); i++)
			{
				std::vector<QuestColliderTrigger>& colliderTriggerList = mViewSubQuest[i].mclearConditionList.colliderTriggerList;
				if (colliderTriggerList.size() > 0)
				{
					if (colliderTriggerList[0].colliderName == event.colliderName)
					{
						colliderTriggerList[0].maxPlayer = event.maxPlayer;
						colliderTriggerList[0].curPlayer = event.curPlayer;
					}
				}
			}
		});
}

void fq::client::QuestManager::setColliderTriggerChecker()
{
	// 메인퀘스트, 서브 퀘스트 목록 돌면서 해당하는 콜라이더에 isall, playernum 설정해주기 
	for (int i = 0; i < mMainQuests.size(); i++)
	{
		std::vector<QuestColliderTrigger>& joinColliderTriggerList = mMainQuests[i].mJoinConditionList.colliderTriggerList;
		if (joinColliderTriggerList.size() > 0)
		{
			// hascomponent로 검사하고 밑에 거 실행하기
			auto object = GetScene()->GetObjectByName(joinColliderTriggerList[0].colliderName);
			if (object != nullptr)
			{
				QuestColliderTriggerChecker* questColliderTriggerChecker = object->GetComponent<QuestColliderTriggerChecker>();
				if (questColliderTriggerChecker != nullptr)
				{
					questColliderTriggerChecker->SetRequestPlayerNum(joinColliderTriggerList[0].isAll, joinColliderTriggerList[0].playerNumber);
				}
			}
		}

		std::vector<QuestColliderTrigger>& clearColliderTriggerList = mMainQuests[i].mclearConditionList.colliderTriggerList;
		if (clearColliderTriggerList.size() > 0)
		{
			auto object = GetScene()->GetObjectByName(clearColliderTriggerList[0].colliderName);
			if (object != nullptr)
			{
				QuestColliderTriggerChecker* questColliderTriggerChecker = object->GetComponent<QuestColliderTriggerChecker>();
				if (questColliderTriggerChecker != nullptr)
				{
					questColliderTriggerChecker->SetRequestPlayerNum(clearColliderTriggerList[0].isAll, clearColliderTriggerList[0].playerNumber);
					clearColliderTriggerList[0].maxPlayer = questColliderTriggerChecker->GetMaxPlayer();
				}
			}
		}
	}
	for (int i = 0; i < mSubQuests.size(); i++)
	{
		std::vector<QuestColliderTrigger>& joinColliderTriggerList = mSubQuests[i].mJoinConditionList.colliderTriggerList;
		if (joinColliderTriggerList.size() > 0)
		{
			auto object = GetScene()->GetObjectByName(joinColliderTriggerList[0].colliderName);
			if (object != nullptr)
			{
				QuestColliderTriggerChecker* questColliderTriggerChecker = object->GetComponent<QuestColliderTriggerChecker>();
				if (questColliderTriggerChecker != nullptr)
				{
					questColliderTriggerChecker->SetRequestPlayerNum(joinColliderTriggerList[0].isAll, joinColliderTriggerList[0].playerNumber);
				}
			}

			//spdlog::trace("QuestManager requestPlayerNum {}, {}, {}",joinColliderTriggerList[0].playerNumber, (int)joinColliderTriggerList[0].isAll);
		}

		std::vector<QuestColliderTrigger>& clearColliderTriggerList = mSubQuests[i].mclearConditionList.colliderTriggerList;
		if (clearColliderTriggerList.size() > 0)
		{
			auto object = GetScene()->GetObjectByName(clearColliderTriggerList[0].colliderName);
			if (object != nullptr)
			{
				QuestColliderTriggerChecker* questColliderTriggerChecker = object->GetComponent<QuestColliderTriggerChecker>();
				if (questColliderTriggerChecker != nullptr)
				{
					questColliderTriggerChecker->SetRequestPlayerNum(clearColliderTriggerList[0].isAll, clearColliderTriggerList[0].playerNumber);
					clearColliderTriggerList[0].maxPlayer = questColliderTriggerChecker->GetMaxPlayer();
				}
			}
		}
	}
}

void fq::client::QuestManager::setMonsterGroup()
{
	for (int i = 0; i < mMainQuests.size(); i++)
	{
		std::vector<MonsterGroupKill>& monsterGroupKillList = mMainQuests[i].mclearConditionList.monsterGroupKillList;
		if (monsterGroupKillList.size() > 0)
		{
			MonsterGroup* monsterGroup = GetScene()->GetObjectByName(monsterGroupKillList[0].monsterGroupName)->GetComponent<MonsterGroup>();
			monsterGroupKillList[0].groupMonsterNumber = monsterGroup->GetAllMonsterSize();
			monsterGroupKillList[0].curNumber = monsterGroup->GetAllMonsterSize();
		}
	}
	for (int i = 0; i < mSubQuests.size(); i++)
	{
		std::vector<MonsterGroupKill>& monsterGroupKillList = mSubQuests[i].mclearConditionList.monsterGroupKillList;
		if (monsterGroupKillList.size() > 0)
		{
			MonsterGroup* monsterGroup = GetScene()->GetObjectByName(monsterGroupKillList[0].monsterGroupName)->GetComponent<MonsterGroup>();
			monsterGroupKillList[0].groupMonsterNumber = monsterGroup->GetAllMonsterSize();
			monsterGroupKillList[0].curNumber = monsterGroup->GetAllMonsterSize();
		}
	}
}

void fq::client::QuestManager::playTimer(int index, Quest& quest, float dt, bool isPlay)
{
	if (quest.mIsTime)
	{
		quest.mSeconds -= dt;

		if (quest.mSeconds <= 0)
		{
			if (isPlay)
			{
				if (mViewSubQuest[index].mSeconds <= 0)
				{
					if (!quest.mIsClear)
					{
						mSubQuestTexts[index]->GetGameObject()->GetParent()->GetChildren()[5]->GetComponent<game_module::TextUI>()->SetIsRender(false);
						mFailImageCounts[index] = 2.0f; // 계속 실행중
						mFailImages[index]->SetIsRender(0, true);
						quest.mIsClear = true;
						for (int i = 0; i < mCurSubQuest.size(); i++)
						{
							if (quest.mIndex == mCurSubQuest[i].mIndex)
							{
								mCurSubQuest.erase(mCurSubQuest.begin() + i);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void fq::client::QuestManager::PlayFail(float dt)
{
	// 페이드 아웃 처리 
	for (int i = 0; i < mFailImageCounts.size(); i++)
	{
		if (mFailImageCounts[i] > 0)
		{
			if (mFailImageCounts[i] < 1.0f)
			{
				auto uiInfo = mFailImages[i]->GetUIInfomation(0);
				uiInfo.Alpha = mFailImageCounts[i];
				mFailImages[i]->SetUIInfomation(0, uiInfo);

				auto textInfo = mSubQuestTexts[i]->GetTextInfo();
				textInfo.FontColor.A(mFailImageCounts[i]);
				mSubQuestTexts[i]->SetTextInfoPlay(textInfo);

				auto questBox = mSubQuestTexts[i]->GetGameObject()->GetParent()->GetChildren()[1]->GetComponent<game_module::ImageUI>();
				uiInfo = questBox->GetUIInfomation(0);
				uiInfo.Alpha = mFailImageCounts[i];
				questBox->SetUIInfomation(0, uiInfo);

				game_module::ImageUI* gaugeBar = mSubQuestTexts[i]->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>();
				gaugeBar->SetIsRender(0, false);

				game_module::ImageUI* gaugeBarBack = nullptr;
				if (gaugeBar->GetGameObject()->GetChildren().size() > 0)
				{
					gaugeBarBack = gaugeBar->GetGameObject()->GetChildren()[0]->GetComponent<game_module::ImageUI>();
					gaugeBarBack->SetIsRender(0, false);
				}

				game_module::TextUI* monsterKillText =
					mSubQuestTexts[i]->GetGameObject()->GetParent()->GetChildren()[2]->GetComponent<game_module::TextUI>();
				monsterKillText->SetIsRender(false);
			}
			mFailImageCounts[i] -= dt;
			mIsFinishedFailedAnimation[i] = true;
		}
		else
		{
			mFailImages[i]->SetIsRender(0, false);
			if (mIsFinishedFailedAnimation[i])
			{
				mIsFinishedFailedAnimation[i] = false;
				mViewSubQuest.erase(mViewSubQuest.begin() + i);

				RenderOnAllSubQuest();
			}
		}
	}
}

void fq::client::QuestManager::eventProcessInProgressDefence()
{
	mInProgressDefenceHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::InProgressDefenceUp>(
		[this](const client::event::InProgressDefenceUp& event)
		{
			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				auto& defenceList = mCurSubQuest[i].mclearConditionList.defenceList;
				if (defenceList.size() > 0)
				{
					if (defenceList[0].colliderName == event.colliderName)
					{
						mIsInProgressDefence[i] = true;
					}
				}
			}
			for (int i = 0; i < mViewSubQuest.size(); i++)
			{
				auto& defenceList = mViewSubQuest[i].mclearConditionList.defenceList;
				if (defenceList.size() > 0)
				{
					if (defenceList[0].colliderName == event.colliderName)
					{
						mIsInProgressDefenceView[i] = true;
					}
				}
			}
		});
}

void fq::client::QuestManager::initSTL()
{
	mCurSubQuest.clear();
	mViewSubQuest.clear();
	mNextSubQuests.clear();
	mClearEvents.clear();
	mClearEventIndexes.clear();
	mIsInProgressDefence.clear();
	mIsInProgressDefenceView.clear();
	mSubQuestTexts.clear();
	mQuestBoxes.clear();
	mNewImages.clear();
	mNewImageCounts.clear();
	mCompleteImages.clear();
	mCompleteImageCounts.clear();
	mLeftChecks.clear();
	mRightChecks.clear();
	mIsFinishedCompleteAnimation.clear();
	mFailImages.clear();
	mFailImageCounts.clear();
	mIsFinishedFailedAnimation.clear();
	mIsAlive.clear();
	mIsUseA.clear();
	mIsUseX.clear();
	mIsUseY.clear();
	mIsUseR.clear();
}

void fq::client::QuestManager::RenderOnAllSubQuest()
{
	for (int i = 0; i < 3; i++)
	{
		if (i < mViewSubQuest.size())
		{
			RenderOnSubQuest(i, true);
		}
		else
		{
			RenderOnSubQuest(i, false);
		}
	}
}

void fq::client::QuestManager::startNew(int index)
{
	mNewImageCounts[index] = 3.0f;
}

void fq::client::QuestManager::startComplete(int index)
{
	if (mCompleteImageCounts[index] <= 0.0f)
	{
		mCompleteImageCounts[index] = 3.0f;
		auto uiInfo = mCompleteImages[index]->GetUIInfomation(0);
		uiInfo.isRender = true;
		uiInfo.Alpha = 1;
		mCompleteImages[index]->SetUIInfomation(0, uiInfo);

		uiInfo = mLeftChecks[index]->GetUIInfomation(0);
		uiInfo.Width = 100;
		uiInfo.Height = 100;
		uiInfo.Alpha = 1;
		uiInfo.isRender = true;
		mLeftChecks[index]->SetUIInfomation(0, uiInfo);

		uiInfo = mRightChecks[index]->GetUIInfomation(0);
		uiInfo.Width = 100;
		uiInfo.Height = 100;
		uiInfo.Alpha = 1;
		uiInfo.isRender = true;
		mRightChecks[index]->SetUIInfomation(0, uiInfo);

		mNewImages[index]->SetIsRender(0, false);
	}
}

void fq::client::QuestManager::eventProcessPushButtonEvent()
{
	mPushButtonHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PushButtonEvent>(
		[this](const client::event::PushButtonEvent& event)
		{
			std::vector<PushButton>& pushButtonList = mCurMainQuest.mclearConditionList.pushButtonList;
			if (pushButtonList.size() > 0)
			{
				if (pushButtonList[0].skillType == event.eSkillType)
				{
					if (pushButtonList[0].isAll)
					{
						int pushButtonPlayer = -1;
						switch (event.eSkillType)
						{
							case ESkillType::X:
								mIsUseX[event.playerID] = true;
								pushButtonPlayer = getXPushPlayer();
								break;
							case ESkillType::A:
								mIsUseA[event.playerID] = true;
								pushButtonPlayer = getAPushPlayer();
								break;
							case ESkillType::R:
								mIsUseR[event.playerID] = true;
								pushButtonPlayer = getRPushPlayer();
								break;
							case ESkillType::Y:
								mIsUseY[event.playerID] = true;
								pushButtonPlayer = getYPushPlayer();
								break;
							default:
								break;
						}

						if (getMaxPlayer() == pushButtonPlayer)
						{
							mClearEvents.push_back(mCurMainQuest);
							mClearEventIndexes.push_back(0);
							//GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							//	{ mCurMainQuest, 0 });
							spdlog::trace("Complete Clear Push Skill");
						}
					}
					else
					{
						mClearEvents.push_back(mCurMainQuest);
						mClearEventIndexes.push_back(0);
						//GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
						//	{ mCurMainQuest, 0 });
						spdlog::trace("Complete Clear Push Skill");
					}
				}
			}

			for (int i = 0; i < mCurSubQuest.size(); i++)
			{
				std::vector<PushButton>& pushButtonList = mCurSubQuest[i].mclearConditionList.pushButtonList;
				if (pushButtonList.size() > 0)
				{
					if (pushButtonList[0].skillType == event.eSkillType)
					{
						if (pushButtonList[0].isAll)
						{
							int pushButtonPlayer = -1;
							switch (event.eSkillType)
							{
								case ESkillType::X:
									mIsUseX[event.playerID] = true;
									pushButtonPlayer = getXPushPlayer();
									break;
								case ESkillType::A:
									mIsUseA[event.playerID] = true;
									pushButtonPlayer = getAPushPlayer();
									break;
								case ESkillType::R:
									mIsUseR[event.playerID] = true;
									pushButtonPlayer = getRPushPlayer();
									break;
								case ESkillType::Y:
									mIsUseY[event.playerID] = true;
									pushButtonPlayer = getYPushPlayer();
									break;
								default:
									break;
							}

							if (getMaxPlayer() == pushButtonPlayer)
							{
								//GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
								//	{ mCurMainQuest, 0 });
								mClearEvents.push_back(mCurSubQuest[i]);
								mClearEventIndexes.push_back(i);
								spdlog::trace("Complete Clear Push Skill");
							}
						}
						else
						{
							//GetScene()->GetEventManager()->FireEvent<client::event::ClearQuestEvent>(
							//	{ mCurMainQuest, 0 });
							mClearEvents.push_back(mCurSubQuest[i]);
							mClearEventIndexes.push_back(i);
							spdlog::trace("Complete Clear Push Skill");
						}
					}
				}
			}
		});
}

int fq::client::QuestManager::getMaxPlayer()
{
	int maxPlayerNum = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsAlive[i] == true)
		{
			maxPlayerNum++;
		}
	}

	return maxPlayerNum;
}

void fq::client::QuestManager::eventProcessUpdatePlayerState()
{
	mUpdatePlayerStateHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::UpdatePlayerState>(
		[this](const client::event::UpdatePlayerState& event)
		{
			if (event.type == EPlayerType::SoulDestoryed)
			{
				mIsAlive[event.playerID] = false;
			}
		});
}

int fq::client::QuestManager::getXPushPlayer()
{
	int maxPlayerNum = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsUseX[i] == true)
		{
			maxPlayerNum++;
		}
	}

	return maxPlayerNum;
}

int fq::client::QuestManager::getRPushPlayer()
{
	int maxPlayerNum = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsUseR[i] == true)
		{
			maxPlayerNum++;
		}
	}

	return maxPlayerNum;
}

int fq::client::QuestManager::getAPushPlayer()
{
	int maxPlayerNum = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsUseA[i] == true)
		{
			maxPlayerNum++;
		}
	}

	return maxPlayerNum;
}

int fq::client::QuestManager::getYPushPlayer()
{
	int maxPlayerNum = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsUseY[i] == true)
		{
			maxPlayerNum++;
		}
	}

	return maxPlayerNum;
}

void fq::client::QuestManager::initPushPlayer(std::vector<bool>& pushVec)
{
	for (int i = 0; i < pushVec.size(); i++)
	{
		pushVec[i] = false;
	}
}

void fq::client::QuestManager::eventProcessOnUIRender()
{
	mUIRenderHandler = GetScene()->GetEventManager()->RegisterHandle<fq::event::UIRender>(
		[this](const fq::event::UIRender& event)
		{
			mIsRenderingUI = event.bIsRenderingUI;
		}
	);
}

