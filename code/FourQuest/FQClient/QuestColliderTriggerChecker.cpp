#include "QuestColliderTriggerChecker.h"

#include "../FQGameModule/Scene.h" 
#include "../FQGameModule/EventManager.h" 
#include "GameManager.h"

#include "ClientEvent.h"
#include "PlayerInfoVariable.h"

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
	mIsClear(false)
{
}

fq::client::QuestColliderTriggerChecker::QuestColliderTriggerChecker(const QuestColliderTriggerChecker& other)
	:mCollidingPlayerNum(0),
	mIsClear(0)
{
}

fq::client::QuestColliderTriggerChecker& fq::client::QuestColliderTriggerChecker::operator=(const QuestColliderTriggerChecker& other)
{
	mCollidingPlayerNum = 0;
	mIsClear = false;

	return *this;
}

fq::client::QuestColliderTriggerChecker::~QuestColliderTriggerChecker()
{
}

void fq::client::QuestColliderTriggerChecker::OnTriggerEnter(const game_module::Collision& collision)
{
	if (!mIsClear)
	{
		if (collision.other->GetTag() == game_module::ETag::Player ||
			collision.other->GetTag() == game_module::ETag::Soul ||
			collision.other->GetTag() == game_module::ETag::Dash)
		{
			mCollidingPlayerNum++;

			int maxPlayerNum = 0;
			for (int i = 0; i < 4; i++)
			{
				if (mIsAlive[i] == true)
				{
					maxPlayerNum++;
				}
			}
			if (maxPlayerNum == mCollidingPlayerNum)
			{
				mIsClear = true;
				// 이벤트 발생
				GetScene()->GetEventManager()->FireEvent<client::event::AllCollideTrigger>(
					{ GetGameObject()->GetName() });
			}
		}
	}
}

void fq::client::QuestColliderTriggerChecker::OnTriggerExit(const game_module::Collision& collision)
{
	if (!mIsClear)
	{
		if (collision.other->GetTag() == game_module::ETag::Player ||
			collision.other->GetTag() == game_module::ETag::Soul ||
			collision.other->GetTag() == game_module::ETag::Dash)
		{
			mCollidingPlayerNum--;
		}
	}
}

void fq::client::QuestColliderTriggerChecker::OnAwake()
{
	mIsAlive.clear();
	for (int i = 0; i < 4; i++)
	{
		mIsAlive.push_back(false);
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

	EventProcessUpdatePlayerState();
}

void fq::client::QuestColliderTriggerChecker::EventProcessUpdatePlayerState()
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

void fq::client::QuestColliderTriggerChecker::OnTriggerStay(const game_module::Collision& collision)
{
	if (!mIsClear)
	{
		int maxPlayerNum = 0;
		for (int i = 0; i < 4; i++)
		{
			if (mIsAlive[i] == true)
			{
				maxPlayerNum++;
			}
		}
		if (maxPlayerNum == mCollidingPlayerNum)
		{
			mIsClear = true;
			// 이벤트 발생
			GetScene()->GetEventManager()->FireEvent<client::event::AllCollideTrigger>(
				{ GetGameObject()->GetName() });
		}
	}
}

