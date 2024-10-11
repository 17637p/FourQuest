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

	if (cloneQuestManager == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneQuestManager = game_module::ObjectPool::GetInstance()->Assign<QuestManager>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

	// ���� ����Ʈ ���
	mCurSubQuest.clear();
	mViewSubQuest.clear();

	mCurMainQuest = mMainQuests[mStartQuests.startMainQuestIndex];
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

	// 
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren();
	mMainQuestText = children[1]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	for (int i = 0; i < 3; i++)
	{
		mSubQuestTexts.push_back(children[3]->GetChildren()[i]->GetChildren()[0]->GetComponent<game_module::TextUI>());
	}

	// GaugeMaxWidth ����
	mGaugeMaxWidth = mMainQuestText->GetGameObject()->GetParent()->GetChildren()[3]->GetComponent<game_module::ImageUI>()->GetUIInfomation(0).Width;
	// �⺻ FontSize ����
	mFontSize = mMainQuestText->GetTextInfo().FontSize;

	// �̺�Ʈ �ڵ鷯 ���
	eventProcessKillMonster();
	eventProcessPlayerCollideTrigger();
	eventProcessCompleteDefence();
	eventProcessClearQuest();
	eventProcessAllColliderTrigger();
	eventProcessObjectInteraction();
	eventProcessClearGoddessStatue();

	mScreenManager = GetScene()->GetScreenManager();

	/// ����� UI
	// New, Complete
	mNewImages.clear();
	mNewImageCounts.clear();
	mCompleteImages.clear();
	mCompleteImageCounts.clear();
	mNextSubQuests.clear();

	mQuestBoxes.clear();

	mNewImages.push_back(children[1]->GetChildren()[4]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	mCompleteImages.push_back(children[1]->GetChildren()[4]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	mQuestBoxes.push_back(children[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	for (int i = 0; i < 3; i++)
	{
		mNewImages.push_back(children[3]->GetChildren()[i]->GetChildren()[4]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mCompleteImages.push_back(children[3]->GetChildren()[i]->GetChildren()[4]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
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

	mLeftChecks.clear();
	mRightChecks.clear();

	mLeftChecks.push_back(children[1]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	mRightChecks.push_back(children[1]->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());

	for (int i = 0; i < 3; i++)
	{
		mLeftChecks.push_back(children[3]->GetChildren()[i]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mRightChecks.push_back(children[3]->GetChildren()[i]->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>());
	}

	// ��������
	for (int i = 0; i < 3 - mCurSubQuest.size(); i++)
	{
		RenderOnSubQuest(2 - i, false);
	}
}

void fq::client::QuestManager::OnUpdate(float dt)
{
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
	}

	GetScene()->GetEventManager()->FireEvent<client::event::CurrentQuest>({ true, mCurMainQuest.mIndex });
	for (int i = 0; i < mCurSubQuest.size(); i++)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::CurrentQuest>({ false, mCurSubQuest[i].mIndex});
	}

	playNew(dt);
	playComplete(dt);
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
}

void fq::client::QuestManager::eventProcessKillMonster()
{
	mKillMonsterHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::KillMonster>(
		[this](const client::event::KillMonster& event)
		{
			// KillMonster ó�� 
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
							{ mCurSubQuest[j], j});
						spdlog::trace("MonsterKill Quest Clear!");
					}
				}
			}

			// Group Monster Kill ó��
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

void fq::client::QuestManager::eventProcessPlayerCollideTrigger()
{
	mPlayerCollideTriggerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerCollideTrigger>(
		[this](const client::event::PlayerCollideTrigger& event)
		{
			// Defence ó��
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

			// Join - Collider Trigger ó��
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

			// Clear - Collider Trigger ó��
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
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Quest_complete", false , fq::sound::EChannel::SE });

			// ��� �� ����Ʈ�� Ŭ���� �������� �ִ� ����Ʈ�� �ִٸ� Ŭ���� �ϱ� 
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
			// ��� �� ����Ʈ�� ��Ż ������ �ִٸ� ���� �ޱ�
			std::vector<RewardPortal> rewardPortalList = event.clearQuest.mRewardList.RewardPortalList;
			int rewardPortalListSize = rewardPortalList.size();
			if (rewardPortalListSize > 0)
			{
				for (int i = 0; i < rewardPortalListSize; i++)
				{
					// prefab���� ��Ż ��ȯ
					std::shared_ptr<game_module::GameObject> portal;

					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPortalPrefab);
					portal = *(instance.begin());

					// Ʈ������ ��ġ �ٲٱ�
					portal->GetComponent<game_module::Transform>()->SetLocalPosition(rewardPortalList[i].position);
					// Portal ������Ʈ�� �����ؼ� ���̸� �ٲٱ�
					portal->GetComponent<Portal>()->SetNextSceneName(rewardPortalList[i].nextSceneName);

					GetScene()->AddGameObject(portal);
				}
			}

			// ��� �� ����Ʈ�� ���� ������ �ִٸ� ���� �ޱ�
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

			// ��� �� ����Ʈ�� ������ ������ �ִٸ� ����
			std::vector<SequenceStart> sequenceStartList = event.clearQuest.mRewardList.SequenceStartList;
			int sequenceStartListSize = sequenceStartList.size();
			if (sequenceStartListSize > 0)
			{
				for (int i = 0; i < sequenceStartListSize; i++)
				{
					GetScene()->GetObjectByName(sequenceStartList[i].name)->GetComponent<game_module::Sequence>()->SetIsPlay(true);
				}
			}

			// ��� �� ����Ʈ�� ��������Ʈ��� ���� ����Ʈ �߰��ϱ�
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

						// Complete ����
						int questUIindex = 0;
						mCompleteImageCounts[questUIindex] = 3.0f;
						auto uiInfo = mCompleteImages[questUIindex]->GetUIInfomation(0);
						uiInfo.isRender = true;
						uiInfo.Alpha = 1;
						mCompleteImages[questUIindex]->SetUIInfomation(0, uiInfo);

						uiInfo = mLeftChecks[questUIindex]->GetUIInfomation(0);
						uiInfo.Width = 100;
						uiInfo.Height = 100;
						uiInfo.Alpha = 1;
						uiInfo.isRender = true;
						mLeftChecks[questUIindex]->SetUIInfomation(0, uiInfo);

						uiInfo = mRightChecks[questUIindex]->GetUIInfomation(0);
						uiInfo.Width = 100;
						uiInfo.Height = 100;
						uiInfo.Alpha = 1;
						uiInfo.isRender = true;
						mRightChecks[questUIindex]->SetUIInfomation(0, uiInfo);

						mNewImages[questUIindex]->SetIsRender(0, false);
					}
				}
			}

			// ���� ����Ʈ�� ��Ͽ��� ���� 
			if (!event.clearQuest.mIsMain)
			{
				mCurSubQuest.erase(mCurSubQuest.begin() + event.index);

				// Complete ����
				if (!event.clearQuest.mIsMain)
				{
					for (int i = 0; i < mViewSubQuest.size(); i++)
					{
						if (mViewSubQuest[i].mIndex == event.clearQuest.mIndex)
						{
							int questUIindex = event.index + 1;
							mCompleteImageCounts[questUIindex] = 3.0f;
							auto uiInfo = mCompleteImages[questUIindex]->GetUIInfomation(0);
							uiInfo.isRender = true;
							uiInfo.Alpha = 1;
							mCompleteImages[questUIindex]->SetUIInfomation(0, uiInfo);

							uiInfo = mLeftChecks[questUIindex]->GetUIInfomation(0);
							uiInfo.Width = 100;
							uiInfo.Height = 100;
							uiInfo.Alpha = 1;
							uiInfo.isRender = true;
							mLeftChecks[questUIindex]->SetUIInfomation(0, uiInfo);

							uiInfo = mRightChecks[questUIindex]->GetUIInfomation(0);
							uiInfo.Width = 100;
							uiInfo.Height = 100;
							uiInfo.Alpha = 1;
							uiInfo.isRender = true;
							mRightChecks[questUIindex]->SetUIInfomation(0, uiInfo);

							mNewImages[questUIindex]->SetIsRender(0, false);
						}
					}
				}
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
	// Clear - Collider Trigger ó��
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
	// textUI children 0 - QuestBox, 1 - monsterKillText, 2 - GaugeBar
	game_module::TextUI* monsterKillText =
		textUI->GetGameObject()->GetParent()->GetChildren()[2]->GetComponent<game_module::TextUI>();
	
	auto text = monsterKillText->GetTextInfo();
	text.IsRender = false;
	monsterKillText->SetTextInfo(text);

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
		if (gaugeBarBack != nullptr)
		{
			gaugeBarBack->SetIsRender(0, true);
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
		auto image2 = subQuest[3]->GetComponent<game_module::ImageUI>();

		textInfo = text2->GetTextInfo();
		textInfo.IsRender = isOn;
		text2->SetTextInfo(textInfo);

		image1->SetIsRender(0, isOn);
		image2->SetIsRender(0, isOn);
	}

	auto uiInfo = mQuestBoxes[i + 1]->GetUIInfomation(0);
	uiInfo.Alpha = 1;
	uiInfo.isRender = isOn;
	mQuestBoxes[i + 1]->SetUIInfomation(0, uiInfo);
}

void fq::client::QuestManager::setScaleAndPositionScreen()
{
	// Scale �ڵ� ���� 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}

	// Text ũ�� �� ��ġ ���� (������ ��)
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
	// Position �ڵ� ����
	{
		myTransform->SetLocalPosition({ 0.85f * screenWidth, 0.45f * screenHeight, 1 });
	}
}

void fq::client::QuestManager::SpawnArmour(fq::game_module::PrefabResource armour)
{
	// ���� ��ȯ
	std::shared_ptr<game_module::GameObject> armourObject;

	//for (int aaa = 0; aaa < 1000; aaa++) ������
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(armour);
		armourObject = *(instance.begin());

		GetScene()->AddGameObject(armourObject);

		/// ���� ��ġ ����
		// ī�޶� �߽� ��������
		DirectX::SimpleMath::Vector3 center = GetScene()->GetObjectByName("MainCamera")->GetComponent<CameraMoving>()->GetCenterCameraInWorld();
		DirectX::SimpleMath::Vector3 nearPos = { 0, 0, 0 };

		// ���� ��ġ ����
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distanceDistribution(0, mDistance);
		std::uniform_real_distribution<float> degreeDistribution(0, 360);

		float randomDistance = distanceDistribution(generator);
		float randomDegree = degreeDistribution(generator);

		float radian = randomDegree * 3.1415926535f / 180.0f;

		center.x += std::cosf(radian) * randomDistance;
		center.z += std::sinf(radian) * randomDistance;

		// ��ȿ�� ��ġ���� Ȯ��
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
		if (mNewImageCounts[i] > 0)
		{
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
	// üũ �Ѵ� ����� 0.5 ���� �۾��� 0.5 ������ �۾��� 1�� �� ���̵� �ƿ�
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
			// Gauge Bar, ���� Text �����
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

			// ���̵� �ƿ� ó�� 
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
					auto textInfo = mMainQuestText->GetTextInfo();
					textInfo.FontColor.A(mCompleteImageCounts[i]);
					mMainQuestText->SetTextInfoPlay(textInfo);
				}
				else
				{
					auto textInfo = mSubQuestTexts[i - 1]->GetTextInfo();
					textInfo.FontColor.A(mCompleteImageCounts[i]);
					mSubQuestTexts[i - 1]->SetTextInfoPlay(textInfo);
				}

				auto questBox = textUI->GetGameObject()->GetParent()->GetChildren()[1]->GetComponent<game_module::ImageUI>();
				uiInfo = questBox->GetUIInfomation(0);
				uiInfo.Alpha = mCompleteImageCounts[i];
				questBox->SetUIInfomation(0, uiInfo);
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
					// New ����
					mNewImageCounts[i] = 3.0f;
					auto uiInfo = mNewImages[i]->GetUIInfomation(0);
					uiInfo.isRender = true;
					uiInfo.Alpha = 1;
					mNewImages[i]->SetUIInfomation(0, uiInfo);

					mIsFinishedCompleteAnimation[i] = false;

					int subQuestSize = mCurSubQuest.size();
					if (mViewSubQuest.size() != subQuestSize)
					{
						mViewSubQuest = mCurSubQuest;

						// New ����
						mNewImageCounts[subQuestSize] = 3.0f;
						auto uiInfo = mNewImages[subQuestSize]->GetUIInfomation(0);
						uiInfo.isRender = true;
						uiInfo.Alpha = 1;
						mNewImages[subQuestSize]->SetUIInfomation(0, uiInfo);
					}
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
			}
			// Sub
			else
			{
				auto textInfo = mSubQuestTexts[i-1]->GetTextInfo();
				textInfo.FontColor.A(1);
				mSubQuestTexts[i - 1]->SetTextInfoPlay(textInfo);

				if (mIsFinishedCompleteAnimation[i])
				{
					if (mViewSubQuest.size() > 0)
					{
						mViewSubQuest.erase(mViewSubQuest.begin() + i - 1);
					}
					if (mNextSubQuests.size() > 0)
					{
						// ���� ����Ʈ �߰�
						mViewSubQuest.push_back(mNextSubQuests.front());
						mNextSubQuests.pop_front();

						// New ����
						mNewImageCounts[i] = 3.0f;
						auto uiInfo = mNewImages[i]->GetUIInfomation(0);
						uiInfo.isRender = true;
						uiInfo.Alpha = 1;
						mNewImages[i]->SetUIInfomation(0, uiInfo);
					}

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

					mIsFinishedCompleteAnimation[i] = false;
				}
			}
		}
	}
}

