#include "GoddessStatue.h"

#include "Player.h"
#include "ArmourSpawner.h"
#include "Soul.h"
#include "HpBar.h"
#include "ClientEvent.h"

#include "../FQGameModule/Transform.h"

#include <spdlog/spdlog.h>

std::shared_ptr<fq::game_module::Component> fq::client::GoddessStatue::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGoddessStatue = std::dynamic_pointer_cast<GoddessStatue>(clone);

	if (cloneGoddessStatue == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneGoddessStatue = game_module::ObjectPool::GetInstance()->Assign<GoddessStatue>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
			// 디버프 장판 이펙트 생성
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerDebuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(player->GetTransform());

			GetScene()->AddGameObject(effectObj);
			mDebuffEffects[player] = effectObj;
		}
		else 
		{
			// 버프 장판 이펙트 생성
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerBuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(player->GetTransform());

			GetScene()->AddGameObject(effectObj);
			mBuffEffects[player] = effectObj;

			buffPlayer(player);
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
			// 디버프 이펙트 삭제
			GetScene()->DestroyGameObject(mDebuffEffects[collisionPlayer].get());
			mDebuffEffects.erase(collisionPlayer);
		}
		else
		{
			// 디버프 이펙트 삭제
			GetScene()->DestroyGameObject(mBuffEffects[collisionPlayer].get());
			mBuffEffects.erase(collisionPlayer);
			releaseBuffPlayer(collisionPlayer);
		}

		// 범위 안 플레이어 목록에서 삭제
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
	mDecreaseSpeed(0.5f),
	mGodDamageBuff(0.5),
	mGodCoolTimeBuff(0.7),
	mGodMoveBuff(0.5)
{
}

fq::client::GoddessStatue::~GoddessStatue()
{
}

void fq::client::GoddessStatue::OnStart()
{
	mIsCorrupt = true;
	mCurGauge = 0;

	GetComponent<HpBar>()->SetVisible(false);
}

void fq::client::GoddessStatue::OnUpdate(float dt)
{
	// 오염된 여신상
	if (mIsCorrupt)
	{
		dealingPlayer(dt);
		cleanUpGoddessState(dt);
	}
}

void fq::client::GoddessStatue::dealingPlayer(float dt)
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

void fq::client::GoddessStatue::cleanUpGoddessState(float dt)
{
	if (mIsOverlayedSoul)
	{
		mCurGauge += dt * mFillGaugeSpeed;
		HpBar* hpBar = GetComponent<HpBar>();
		hpBar->SetVisible(true);
		hpBar->SetHp(mCurGauge / mMaxGauge);
		spdlog::trace("{}", mCurGauge / mMaxGauge);
		if (mCurGauge > mMaxGauge)
		{
			hpBar->SetVisible(false);

			// 정화 완료
			mIsCorrupt = false;
			spawnArmour();
			mCurOverlaySoul->ReleaseGoddessStatue();

			// 오염 이펙트 삭제하고 정화 이펙트 재생
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mBuff);
			auto& effectObj = *(instance.begin());
			auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
			buffEffectT->SetParent(GetTransform());

			GetScene()->DestroyGameObject(GetGameObject()->GetChildren()[1]);
			GetScene()->AddGameObject(effectObj);

			// 범위안 플레이어 오염 장판 지우고 정화 장판 붙이기 
			for (auto& player : mInRangePlayers)
			{
				GetScene()->DestroyGameObject(mDebuffEffects[player].get());
			}
			mDebuffEffects.clear();

			for (auto& player : mInRangePlayers)
			{
				// 버프 장판 이펙트 생성
				auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerBuff);
				auto& effectObj = *(instance.begin());
				auto buffEffectT = effectObj->GetComponent<game_module::Transform>();
				buffEffectT->SetParent(player->GetTransform());

				GetScene()->AddGameObject(effectObj);
				mBuffEffects[player] = effectObj;
			}

			// 퀘스트 클리어를 위한 이벤트 발생
			GetScene()->GetEventManager()->FireEvent<client::event::ClearGoddessStatue>(
				{ GetGameObject()->GetName() });
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

void fq::client::GoddessStatue::spawnArmour()
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

void fq::client::GoddessStatue::buffPlayer(Player* player)
{
	// 쿨타임 감소 
	player->SetGBDecreaseCooltime(mGodCoolTimeBuff);
	// 내구도 감소
	player->SetGBDecreaseDurability(true);
	// 공격력 증가
	player->SetGBIncreaseAttackPower(mGodDamageBuff);
	// 이동속도 증가
	player->SetGBIncreaseSpeed(mGodMoveBuff);
}

void fq::client::GoddessStatue::releaseBuffPlayer(Player* player)
{
	// 쿨타임 감소 
	player->SetGBDecreaseCooltime(1);
	// 내구도 감소
	player->SetGBDecreaseDurability(false);
	// 공격력 증가
	player->SetGBIncreaseAttackPower(0);
	// 이동속도 증가
	player->SetGBIncreaseSpeed(0);
}

// 영혼 빙의 처리 (1명만 빙의 가능)
// 영혼 빙의 해제 처리