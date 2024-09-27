#include "GoddessStatue.h"

#include "Player.h"
#include "ArmourSpawner.h"
#include "Soul.h"
#include "HpBar.h"

#include <spdlog/spdlog.h>

std::shared_ptr<fq::game_module::Component> fq::client::GoddessStatue::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGoddessStatue = std::dynamic_pointer_cast<GoddessStatue>(clone);

	if (cloneGoddessStatue == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneGoddessStatue = game_module::ObjectPool::GetInstance()->Assign<GoddessStatue>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneGoddessStatue = *this;
	}

	return cloneGoddessStatue;
}

void fq::client::GoddessStatue::OnCollisionEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		mInRangePlayer.push_back(collision.other->GetComponent<Player>());
	}
}

void fq::client::GoddessStatue::OnCollisionExit(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		Player* collisionPlayer = collision.other->GetComponent<Player>();
		int collisionPlayerID = collisionPlayer->GetPlayerID();

		mInRangePlayer.erase(std::remove(mInRangePlayer.begin(), mInRangePlayer.end(), collisionPlayer)
			, mInRangePlayer.end()
		);
	}
}

fq::client::GoddessStatue::GoddessStatue()
	:mIsCorrupt(true),
	mIsOverlayedSoul(false),
	mCurOverlaySoul(nullptr),
	mInRangePlayer(),
	mCurGauge(0),
	mDealingCoolTime(0),
	mDealingDamage(0),
	mMaxGauge(10),
	mFillGaugeSpeed(1),
	mDecreaseSpeed(0.5f)
{
}

fq::client::GoddessStatue::~GoddessStatue()
{
}

void fq::client::GoddessStatue::OnStart()
{
	mIsCorrupt = true;
	mCurGauge = 0;
}

void fq::client::GoddessStatue::OnUpdate(float dt)
{
	// ������ ���Ż�
	if (mIsCorrupt)
	{
		DealingPlayer(dt);
		CleanUpGoddessState(dt);
	}
}

void fq::client::GoddessStatue::DealingPlayer(float dt)
{
	mDealingCoolTime += dt;
	if (mDealingCoolTime > mDealingTime)
	{
		mDealingCoolTime = 0;

		for (auto& player : mInRangePlayer)
		{
			//player->Damage(mDealingDamage);
		}
	}
}

void fq::client::GoddessStatue::CleanUpGoddessState(float dt)
{
	if (mIsOverlayedSoul)
	{
		mCurGauge += dt * mFillGaugeSpeed;
		GetComponent<HpBar>()->SetHp(mCurGauge / mMaxGauge);
		spdlog::trace("{}", mCurGauge / mMaxGauge);
		if (mCurGauge > mMaxGauge)
		{
			mIsCorrupt = false;
			SpawnArmour();
			mCurOverlaySoul->ReleaseGoddessStatue();
		}
	}
	else
	{
		mCurGauge -= dt * mDecreaseSpeed;
		if (mCurGauge < 0)
		{
			mCurGauge = 0;
		}
	}
}

void fq::client::GoddessStatue::SpawnArmour()
{
	GetComponent<ArmourSpawner>()->SpawnArmourAll();
}

bool fq::client::GoddessStatue::SetOverlaySoul(bool isOverlay, Soul* soul)
{
	if (isOverlay)
	{
		if (!mIsOverlayedSoul)
		{
			mIsOverlayedSoul = true;
			mCurOverlaySoul = soul;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mIsOverlayedSoul = false;
		return false;
	}
}

// ��ȥ ���� ó�� (1�� ���� ����)
// ��ȥ ���� ���� ó��