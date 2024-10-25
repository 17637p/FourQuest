#include "DefenceCounter.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"

#include "PlayerInfoVariable.h"
#include "ClientEvent.h"

fq::client::DefenceCounter::DefenceCounter()
	:mCollidingPlayerNum(0),
	mCurCount(0),
	mRequestCount(0),
	mCountSpeed(1),
	mIsClear(false),
	mIsStart(false)
{
}

fq::client::DefenceCounter::~DefenceCounter()
{
}

void fq::client::DefenceCounter::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player ||
		collision.other->GetTag() == game_module::ETag::Soul || 
		collision.other->GetTag() == game_module::ETag::Dash ||
		collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
	{
		mCollidingPlayerNum++;
	}
}

void fq::client::DefenceCounter::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player ||
		collision.other->GetTag() == game_module::ETag::Soul ||
		collision.other->GetTag() == game_module::ETag::Dash ||
		collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
	{
		mCollidingPlayerNum--;
	}
}

void fq::client::DefenceCounter::OnUpdate(float dt)
{
	if (!mIsClear && mIsStart)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::InProgressDefence>(
			{ GetGameObject()->GetName(), (int)mCurCount });

		if (mCollidingPlayerNum > 0)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::InProgressDefenceUp>(
				{ GetGameObject()->GetName()});
		}

		mCurCount += dt * mCountSpeed * 4 * ((float)mCollidingPlayerNum) / getMaxPlayer();

		if (mCurCount > mRequestCount)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::CompleteDefence>(
				{ GetGameObject()->GetName() });
			mIsClear = true;
		}
	}
}

std::shared_ptr<fq::game_module::Component> fq::client::DefenceCounter::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneDefence = std::dynamic_pointer_cast<DefenceCounter>(clone);

	if (cloneDefence == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneDefence = game_module::ObjectPool::GetInstance()->Assign<DefenceCounter>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneDefence = *this;
	}

	return cloneDefence;
}

void fq::client::DefenceCounter::OnAwake()
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
}

int fq::client::DefenceCounter::getMaxPlayer()
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
