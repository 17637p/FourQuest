#include "QuestColliderTriggerChecker.h"

#include "../FQGameModule/Scene.h" 
#include "GameManager.h"

#include "ClientEvent.h"

std::shared_ptr<fq::game_module::Component> fq::client::QuestColliderTriggerChecker::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneQuestColliderTrigger = std::dynamic_pointer_cast<QuestColliderTriggerChecker>(clone);

	if (cloneQuestColliderTrigger == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneQuestColliderTrigger = game_module::ObjectPool::GetInstance()->Assign<QuestColliderTriggerChecker>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneQuestColliderTrigger = *this;
	}

	return cloneQuestColliderTrigger;
}

fq::client::QuestColliderTriggerChecker::QuestColliderTriggerChecker()
	:mCollidingPlayerNum(0),
	mIsClear(false),
	mMaxPlayerNum(0)
{
}

fq::client::QuestColliderTriggerChecker::~QuestColliderTriggerChecker()
{
}

void fq::client::QuestColliderTriggerChecker::OnTriggerEnter(const game_module::Collision& collision)
{
	if (!mIsClear)
	{
		if (collision.other->GetTag() == game_module::ETag::Player ||
			collision.other->GetTag() == game_module::ETag::Soul)
		{
			mCollidingPlayerNum++;
		}

		if (mMaxPlayerNum == mCollidingPlayerNum)
		{
			mIsClear = true;
			// 이벤트 발생
			GetScene()->GetEventManager()->FireEvent<client::event::AllCollideTrigger>(
				{ GetGameObject()->GetName() });
		}
	}
}

void fq::client::QuestColliderTriggerChecker::OnTriggerExit(const game_module::Collision& collision)
{
	if (!mIsClear)
	{
		if (collision.other->GetTag() == game_module::ETag::Player ||
			collision.other->GetTag() == game_module::ETag::Soul)
		{
			mCollidingPlayerNum--;
		}
	}
}

void fq::client::QuestColliderTriggerChecker::OnUpdate(float dt)
{
	// 임시 코드
	auto gameManager = GetScene()->GetObjectByName("GameManager");
	auto gameManagerComponent = gameManager->GetComponent<GameManager>();
	mMaxPlayerNum = gameManagerComponent->GetPlayers().size();
}
