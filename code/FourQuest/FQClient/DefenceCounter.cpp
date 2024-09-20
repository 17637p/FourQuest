#include "DefenceCounter.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"

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
		collision.other->GetTag() == game_module::ETag::Soul)
	{
		mCollidingPlayerNum++;
	}
}

void fq::client::DefenceCounter::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player ||
		collision.other->GetTag() == game_module::ETag::Soul)
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

		mCurCount += dt * mCountSpeed * mCollidingPlayerNum;
		spdlog::trace("CurCount: {}, RequestCount: {}", mCurCount, mRequestCount);

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
