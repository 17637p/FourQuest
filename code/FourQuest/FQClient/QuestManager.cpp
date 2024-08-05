#include "QuestManager.h"

fq::client::QuestManager::QuestManager()
	:mCurMainQuest(),
	mCurSubQuest(),
	mMainQuests(),
	mSubQuests()
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
