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

	if (cloneQuestManager == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneQuestManager = game_module::ObjectPool::GetInstance()->Assign<QuestManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneQuestManager = *this;
	}

	/*cloneQuestManager->mCurSubQuest.clear();
	cloneQuestManager->mMainQuests.clear();
	cloneQuestManager->mSubQuests.clear();*/

	return cloneQuestManager;
}
