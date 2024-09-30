#include "GoddessStatue.h"

#include "Player.h"
#include "ArmourSpawner.h"
#include "Soul.h"
#include "HpBar.h"

#include "../FQGameModule/Transform.h"

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

void fq::client::GoddessStatue::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		Player* player = collision.other->GetComponent<Player>();
		mInRangePlayers.push_back(player);
		
		if (mIsCorrupt)
		{
			// ����� ���� ����Ʈ ����
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerDebuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(player->GetTransform());

			GetScene()->AddGameObject(effectObj);
			mDebuffEffects[player] = effectObj;
		}
		else
		{
			// ���� ���� ����Ʈ ����
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerBuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(player->GetTransform());

			GetScene()->AddGameObject(effectObj);
			mBuffEffects[player] = effectObj;
		}
	}
}

void fq::client::GoddessStatue::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		Player* collisionPlayer = collision.other->GetComponent<Player>();
		int collisionPlayerID = collisionPlayer->GetPlayerID();

		if (mIsCorrupt)
		{
			// ����� ����Ʈ ����
			GetScene()->DestroyGameObject(mDebuffEffects[collisionPlayer].get());
			mDebuffEffects.erase(collisionPlayer);
		}
		else
		{
			// ����� ����Ʈ ����
			GetScene()->DestroyGameObject(mBuffEffects[collisionPlayer].get());
			mBuffEffects.erase(collisionPlayer);
		}

		// ���� �� �÷��̾� ��Ͽ��� ����
		mInRangePlayers.erase(std::remove(mInRangePlayers.begin(), mInRangePlayers.end(), collisionPlayer)
			, mInRangePlayers.end()
		);
	}
}

fq::client::GoddessStatue::GoddessStatue()
	:mIsCorrupt(true),
	mIsOverlayedSoul(false),
	mCurOverlaySoul(nullptr),
	mInRangePlayers(),
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

		for (auto& player : mInRangePlayers)
		{
			player->DecreaseHp(mDealingDamage);
			player->SetPoisonRimLight(0.25f);
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
			// ��ȭ �Ϸ�
			mIsCorrupt = false;
			SpawnArmour();
			mCurOverlaySoul->ReleaseGoddessStatue();

			// ���� ����Ʈ �����ϰ� ��ȭ ����Ʈ ���
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mBuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(GetTransform());

			GetScene()->DestroyGameObject(GetGameObject()->GetChildren()[1]);
			GetScene()->AddGameObject(effectObj);

			// ������ �÷��̾� ���� ���� ����� ��ȭ ���� ���̱� 
			for (auto& player : mInRangePlayers)
			{
				GetScene()->DestroyGameObject(mDebuffEffects[player].get());
			}
			mDebuffEffects.clear();

			for (auto& player : mInRangePlayers)
			{
				// ���� ���� ����Ʈ ����
				auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerBuff);
				auto& effectObj = *(instance.begin());
				auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
				buffEffectT->SetParent(player->GetTransform());

				GetScene()->AddGameObject(effectObj);
				mBuffEffects[player] = effectObj;
			}
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

bool fq::client::GoddessStatue::GetIsCorrupt()
{
	return mIsCorrupt;
}

// ��ȥ ���� ó�� (1�� ���� ����)
// ��ȥ ���� ���� ó��