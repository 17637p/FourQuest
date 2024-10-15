#include "QuestColliderTriggerChecker.h"

#include "../FQGameModule/Scene.h" 
#include "../FQGameModule/EventManager.h" 
#include "GameManager.h"

#include "ClientEvent.h"
#include "PlayerInfoVariable.h"

std::shared_ptr<fq::game_module::Component> fq::client::QuestColliderTriggerChecker::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneQuestColliderTrigger = std::dynamic_pointer_cast<QuestColliderTriggerChecker>(clone);

	if (cloneQuestColliderTrigger == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneQuestColliderTrigger = game_module::ObjectPool::GetInstance()->Assign<QuestColliderTriggerChecker>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

fq::client::QuestColliderTriggerChecker::QuestColliderTriggerChecker(const QuestColliderTriggerChecker& other)
	:mCollidingPlayerNum(0),
	mIsClear(0),
	mMaxPlayerNum(0)
{
}

fq::client::QuestColliderTriggerChecker& fq::client::QuestColliderTriggerChecker::operator=(const QuestColliderTriggerChecker& other)
{
	mCollidingPlayerNum = 0;
	mIsClear = false;
	mMaxPlayerNum = 0;

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
			collision.other->GetTag() == game_module::ETag::Dash ||
			collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
		{
			mCollidingPlayerNum++;
		}

		if (mMaxPlayerNum == mCollidingPlayerNum)
		{
			mIsClear = true;
			// �̺�Ʈ �߻�
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
			collision.other->GetTag() == game_module::ETag::Soul ||
			collision.other->GetTag() == game_module::ETag::Dash ||
			collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
		{
			mCollidingPlayerNum--;
		}
	}
}

void fq::client::QuestColliderTriggerChecker::OnAwake()
{
	mMaxPlayerNum = 0;

	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		mMaxPlayerNum++;
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		mMaxPlayerNum++;
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		mMaxPlayerNum++;
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		mMaxPlayerNum++;
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
				mMaxPlayerNum--;
			}
		});
}

