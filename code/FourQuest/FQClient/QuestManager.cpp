#include "QuestManager.h"
#include "ClientEvent.h"

#include <spdlog/spdlog.h>

fq::client::QuestManager::QuestManager()
	:mCurMainQuest(),
	mCurSubQuest(),
	mMainQuests(),
	mSubQuests()
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

	/*cloneQuestManager->mCurSubQuest.clear();
	cloneQuestManager->mMainQuests.clear();
	cloneQuestManager->mSubQuests.clear();*/

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

	EventProcessKillMonster();
}

void fq::client::QuestManager::OnUpdate(float dt)
{
	// Quest Clear
	//if()
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
					spdlog::trace("Quest Clear!");
				}
			}

			// Sub
			for (int j = 0; j < mCurSubQuest.size(); j++)
			{
				std::vector<MonsterKill> monsterKillList = mSubQuests[j].mclearConditionList.monsterKillList;
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
						spdlog::trace("Quest Clear!");
					}
				}
			}
		});
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

