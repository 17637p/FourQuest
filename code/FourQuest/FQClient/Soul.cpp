#include "Soul.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Particle.h"
#include "..\FQGameModule\RigidBody.h"
#include "..\FQGameModule\BoxCollider.h"

#include "DeadArmour.h"
#include "GoddessStatue.h"
#include "CameraMoving.h"
#include "ClientEvent.h"
#include "PlayerSoulVariable.h"
#include "Player.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"
#include "PlayerHPBar.h"

#include "SoulVariable.h"

#include <algorithm>

fq::client::Soul::Soul()
	:mController(nullptr)
	, mSelectArmours{}
	, mbIsDistanceInPlayer(false)
	, mHP()
	, mSelectGoddessStatue(nullptr)
	, mNeedHoldB(2)
	, mCurHoldB(0)
	, mIsOverlayGoddessStatue(false)
	, mbIsOnSummon(false)
	, mSummonArmourOrNull(nullptr)
	, mSoulGauge(0.f)
	, mMaxSoulGauge(100.f)
	, mPlayerHpBar(nullptr)
	, mDashSpeed(0.3f)
	, mDashElapsed(0.f)
	, mDashCoolTime(0.3f)
{}

fq::client::Soul::~Soul()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Soul::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSoul = std::dynamic_pointer_cast<Soul>(clone);

	if (cloneSoul == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneSoul = game_module::ObjectPool::GetInstance()->Assign<Soul>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSoul = *this;
	}

	return cloneSoul;
}

void fq::client::Soul::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();

	// Player등록
	GetScene()->GetEventManager()->FireEvent<client::event::RegisterPlayer>(
		{ GetGameObject(), EPlayerType::Soul });

	// 카메라에 플레이어 등록 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

	// UI 등록
	for (auto& child : GetGameObject()->GetChildren())
	{
		auto ui = child->GetComponent<BGaugeUI>();

		if (ui)
			mBGaugeUI = ui;

		auto hpBar = child->GetComponent<PlayerHPBar>();
		if (hpBar)
		{
			mPlayerHpBar = hpBar;
		}
	}

	SetSoulColor();		// 소울 색깔 지정 
	setSoulHP();		// 소울 HP

	setName();

	mbIsOnSummon = false;
	mSummonArmourOrNull = nullptr;
	mDashElapsed = 0.f;
}

void fq::client::Soul::DestorySoul()
{
	GetScene()->DestroyGameObject(GetGameObject());

	// 카메라에 플레이어 해제 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Soul::OnTriggerEnter(const fq::game_module::Collision& collision)
{
	// deadArmour 처리
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();
	if (deadArmour != nullptr)
	{
		mSelectArmours.push_back(deadArmour);
	}

	// GoddessStatue 처리
	auto goddessStatue = collision.other->GetComponent<fq::client::GoddessStatue>();
	if (goddessStatue != nullptr)
	{
		mSelectGoddessStatue = goddessStatue;
	}

	// Quest Event 
	if (mController != nullptr)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::PlayerCollideTrigger>(
			{ (int)mController->GetControllerID(), collision.other->GetName() });
	}

	if (collision.other->GetTag() == fq::game_module::ETag::SafeZone)
	{
		mbIsInSafeZone = true;
	}
}

void fq::client::Soul::OnTriggerExit(const fq::game_module::Collision& collision)
{
	// deadArmour 처리
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();
	if (deadArmour != nullptr)
	{
		mSelectArmours.erase(std::remove(mSelectArmours.begin(), mSelectArmours.end(), deadArmour)
			, mSelectArmours.end()
		);
	}

	// GoddessStatue 처리
	auto goddessStatue = collision.other->GetComponent<fq::client::GoddessStatue>();
	if (goddessStatue != nullptr)
	{
		mSelectGoddessStatue = nullptr;
	}

	if (collision.other->GetTag() == fq::game_module::ETag::SafeZone)
	{
		mbIsInSafeZone = false;
	}
}

void fq::client::Soul::OnUpdate(float dt)
{
	mbIsVisibleBGaugeUI = false;
	mBGaugeFillAmount = 0.f;

	if (!handleOnSummon())
	{
		processInput(dt); // 가장 먼저 호출 필요 (캐릭터 이동설정)
		selectGoddessStatue(dt);
		selectArmour();
		checkOtherPlayer();
		updateSoulHP(dt);
		checkReleaseGoddessStatue();
	}

	mBGaugeUI->SetVisible(mbIsVisibleBGaugeUI);
	mBGaugeUI->SetRatio(mBGaugeFillAmount);
}

void fq::client::Soul::OnLateUpdate(float dt)
{
	mbIsDistanceInPlayer = false;
}

void fq::client::Soul::selectArmour()
{
	// 오염된 여신상과 상호작용 중 갑옷 빙의 불가능함
	if (mSelectGoddessStatue != nullptr
		&& mSelectGoddessStatue->GetIsCorrupt())
	{
		return;
	}

	auto input = GetScene()->GetInputManager();

	if (!mSelectArmours.empty())
	{
		// 가장 가까운 갑옷 쿼리
		auto soulPos = GetComponent<game_module::Transform>()->GetWorldPosition();
		float minDistance = 100000.f;
		DeadArmour* closestArmour = nullptr;

		for (auto& armour : mSelectArmours)
		{
			if (!armour->IsSummonAble())
				continue;

			auto pos = armour->GetComponent<game_module::Transform>()->GetWorldPosition();
			float distance = (soulPos - pos).Length();

			if (minDistance > distance)
			{
				minDistance = distance;
				closestArmour = armour;
			}
		}

		if (closestArmour == nullptr)
		{
			return;
		}

		// 갑옷을 입을 수 있는지 쿨타임 체크 ( UI 변수 제어 )
		if (mBGaugeUI)
		{
			mbIsVisibleBGaugeUI = true;

			if (closestArmour->GetUnequippedPlayerId() == mController->GetControllerID())
			{
				float ratio = closestArmour->GetPlayerArmourCoolTime() / SoulVariable::ArmourCoolTime * 360.f;
				mBGaugeFillAmount = ratio;
			}
			else
			{
				mBGaugeFillAmount = 360.f;
			}
		}

		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold)
			&& !mIsOverlayGoddessStatue) // 여신상 위치 근처에 갑옷 있을 때 여신상으로 이동하고 갑옷 먹어져서 막음
		{
			PlayerInfo info{ mController->GetControllerID(), mSoulType };

			if (closestArmour->EnterSummonLivingArmour(info))
			{
				mbIsOnSummon = true;
				mSummonArmourOrNull = closestArmour;
				mController->SetCanMoveCharater(false);

				// 위치 조정
				auto armourTransform = closestArmour->GetTransform();
				auto curTransform = GetTransform();
				curTransform->SetWorldMatrix(armourTransform->GetWorldMatrix());

				// 속도 제거
				auto rigidbody = GetComponent<fq::game_module::RigidBody>();
				if (rigidbody != nullptr)
				{
					rigidbody->SetLinearVelocity({ 0,0,0 });
				}
				
				// 사운드 재생
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_ArmorSpawn", false , fq::sound::EChannel::SE });

				mPlayerHpBar->SetVisible(false);
			}
		}
	}
}

void fq::client::Soul::checkOtherPlayer()
{
	// 거리 내에 갑옷을 입은 다른 플레이어를 탐색 후 일정 거리 내에 있으면 영혼 버프
	for (auto& object : GetScene()->GetComponentView<Player>())
	{
		auto myTransform = GetComponent<fq::game_module::Transform>();
		auto otherTransform = object.GetComponent<fq::game_module::Transform>();
		auto otherPlayerComponent = object.GetComponent<Player>();

		DirectX::SimpleMath::Vector3 distanceVector = myTransform->GetWorldPosition() - otherTransform->GetWorldPosition();

		if (distanceVector.Length() <= SoulVariable::SoulDistance)
		{
			mbIsDistanceInPlayer = true;
			int buffNumber = otherPlayerComponent->GetSoulBuffNumber();
			buffNumber++;
			otherPlayerComponent->SetSoulBuffNumber(buffNumber);
		}
	}
}

void fq::client::Soul::SetSoulColor()
{
	if (GetGameObject() == nullptr)
		return;

	for (auto child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::Particle>())
		{
			auto particle = child->GetComponent<game_module::Particle>();
			auto matInfo = particle->GetParticleMaterialInfo();

			switch (mSoulType)
			{
				case fq::client::ESoulType::Sword:
					matInfo.EmissiveColor = PlayerSoulVariable::SwordSoulColor;
					break;
				case fq::client::ESoulType::Staff:
					matInfo.EmissiveColor = PlayerSoulVariable::StaffSoulColor;
					break;
				case fq::client::ESoulType::Axe:
					matInfo.EmissiveColor = PlayerSoulVariable::AxeSoulColor;
					break;
				case fq::client::ESoulType::Bow:
					matInfo.EmissiveColor = PlayerSoulVariable::BowSoulColor;
					break;
			}

			particle->SetParticleMaterialInfo(matInfo);
		}
	}
}

void fq::client::Soul::setSoulHP()
{
	int maxHP = getMaxHP();
	int minHP = SoulVariable::SoulMinHp;

	mHP = std::max<int>(maxHP, minHP);
	if (mPlayerHpBar)
		mPlayerHpBar->DecreaseHp((SoulVariable::SoulMaxHp - mHP) / (float)SoulVariable::SoulMaxHp);
}

void fq::client::Soul::updateSoulHP(float dt)
{
	// 여신상에 빙의 중이거나 세이프 존에 있는 경우
	if (mIsOverlayGoddessStatue || mbIsInSafeZone)
	{
		return;
	}

	// 영혼 주변에 갑옷 플레이어가 있는지 체크 여부에 따라 HP 감소량 감소
	if (mbIsDistanceInPlayer)
	{
		float decreasPercentage = ((100.f - SoulVariable::SoulDecreasPercentage) / 100.f);
		float decreasDamage = SoulVariable::SoulHpDecreas * dt * decreasPercentage;

		mHP -= decreasDamage;
		if (mPlayerHpBar)
			mPlayerHpBar->DecreaseHp((decreasDamage / (float)SoulVariable::SoulMaxHp));
	}
	else
	{
		mHP -= SoulVariable::SoulHpDecreas * dt;
		if (mPlayerHpBar)
			mPlayerHpBar->DecreaseHp((SoulVariable::SoulHpDecreas * dt) / (float)SoulVariable::SoulMaxHp);
	}

	// 영혼 죽으면 오브젝트 삭제하고 소울 매니저한테 영혼 파괴되었다고 알림
	if (mHP <= 0.f)
	{
		DestorySoul();
		int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ id, EPlayerType::SoulDestoryed });
	}
}

bool fq::client::Soul::handleOnSummon()
{
	if (mbIsOnSummon)
	{
		assert(mSummonArmourOrNull != nullptr);
		PlayerInfo info{ mController->GetControllerID(), mSoulType, mSoulGauge };

		if (mPlayerHpBar)
			mPlayerHpBar->SetVisible(false);

		if (mSummonArmourOrNull->SummonLivingArmour(info))
		{
			mController->SetCanMoveCharater(true);
			mController->SetOnMove(true);
			
			DestorySoul();
			spdlog::trace("DestroySoul");
		}
	}

	return mbIsOnSummon;
}

void fq::client::Soul::SetSoulType(fq::client::ESoulType type)
{
	mSoulType = type;

	SetSoulColor();
}

void fq::client::Soul::selectGoddessStatue(float dt)
{
	// 충돌된 여신상이 없거나 정화된 여신상이라면 빠른 종료
	if (mSelectGoddessStatue == nullptr
		|| !mSelectGoddessStatue->GetIsCorrupt())
	{
		mCurHoldB = 0;
		return;
	}

	// 입력 처리
	auto input = GetScene()->GetInputManager();

	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold))
	{
		mCurHoldB += dt;

		if (mCurHoldB > mNeedHoldB)
		{
			mCurHoldB = 0;
			bool isSuccessOverlay = mSelectGoddessStatue->SetOverlaySoul(true, this);

			if (isSuccessOverlay)
			{
				mIsOverlayGoddessStatue = true;
				// 빙의하면 못 움직이게 처리 
				GetComponent<game_module::CharacterController>()->SetCanMoveCharater(false);

				if (mPlayerHpBar)
					mPlayerHpBar->SetVisible(false);

				// 소울 위치를 여신상 위치로 해서 숨기기 
				auto soulT = GetComponent<game_module::Transform>();
				auto goddessStatuePos = mSelectGoddessStatue->GetComponent<game_module::Transform>()->GetWorldPosition();

				goddessStatuePos.y += 2.0f;
				soulT->SetWorldPosition(goddessStatuePos);
			}
		}

	}
	else
	{
		mCurHoldB = 0;
	}

	// 게이지 UI 변수 제어
	mbIsVisibleBGaugeUI = true;
	mBGaugeFillAmount = mCurHoldB / mNeedHoldB * 360.f;
}

void fq::client::Soul::ReleaseGoddessStatue()
{
	mIsOverlayGoddessStatue = false;
	GetComponent<game_module::CharacterController>()->SetCanMoveCharater(true);

	// 여신상 살짝 앞으로 소환
	auto soulT = GetTransform();
	DirectX::SimpleMath::Vector3 forwardPos = soulT->GetWorldPosition();
	forwardPos.z -= 1.0f;
	soulT->SetWorldPosition(forwardPos);

	mSelectGoddessStatue->SetOverlaySoul(false, this);

	if (mPlayerHpBar)
		mPlayerHpBar->SetHPRatio(1.f);


	setSoulHP();
}

void fq::client::Soul::checkReleaseGoddessStatue()
{
	if (mIsOverlayGoddessStatue)
	{
		auto input = GetScene()->GetInputManager();
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
		{
			// 영혼 최대 체력으로 
			ReleaseGoddessStatue();
		}

		// 게이지 UI 변수 제어
		mbIsVisibleBGaugeUI = false;
	}
}

float fq::client::Soul::GetSoulHpRatio() const
{
	return mHP / static_cast<float>(SoulVariable::SoulMaxHp);
}

void fq::client::Soul::setName()
{
	int soulType = static_cast<int>(GetSoulType());
	SpeechBubbleUI* speechBubble = nullptr;
	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<SpeechBubbleUI>())
		{
			speechBubble = child->GetComponent<SpeechBubbleUI>();
		}
	}

	if (speechBubble != nullptr)
	{
		switch (soulType)
		{
			case 0:
				speechBubble->SetName(PlayerInfoVariable::KnightName);
				break;
			case 1:
				speechBubble->SetName(PlayerInfoVariable::MagicName);
				break;
			case 2:
				speechBubble->SetName(PlayerInfoVariable::BerserkerName);
				break;
			case 3:
				speechBubble->SetName(PlayerInfoVariable::ArcherName);
				break;
			default:
				break;
		}
	}
}

float fq::client::Soul::getMaxHP()
{
	// 갑옷 죽음 카운터에 따라 영혼 최대 체력 세팅
	int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();
	int deathCount = 0;

	if (id == 0)
		deathCount = SoulVariable::Player1DeathCount;
	else if (id == 1)
		deathCount = SoulVariable::Player2DeathCount;
	else if (id == 2)
		deathCount = SoulVariable::Player3DeathCount;
	else if (id == 3)
		deathCount = SoulVariable::Player4DeathCount;

	return SoulVariable::SoulMaxHp - SoulVariable::SoulHpDown * deathCount;
}

void fq::client::Soul::processInput(float dt)
{
	auto input = GetScene()->GetInputManager();

	mDashElapsed += dt;

	auto rigidbody = GetComponent<fq::game_module::RigidBody>();
	auto foward = GetTransform()->GetWorldMatrix().Forward();
	foward.Normalize();

	if (mDashCoolTime < mDashElapsed && !GetGameObject()->IsDestroyed())
	{
		mController->SetCanMoveCharater(true);

		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap))
		{
			// 속도 처리
			if (rigidbody != nullptr)
			{
				auto velocity = foward;
				velocity.x *= mDashSpeed;
				velocity.z *= mDashSpeed;
				velocity.y = 0.f;
				rigidbody->SetLinearVelocity(velocity);
				mController->SetCanMoveCharater(false);
			}

			mDashElapsed = 0.f;
		}
	}
}

void fq::client::Soul::SetInvincible()
{
	mbIsInSafeZone = true;
}

float fq::client::Soul::GetHP() const
{
	return mHP;
}

void fq::client::Soul::SetHP(float hp)
{
	if (hp < 0.f)
	{
		return;
	}

	float maxHP = getMaxHP();
	mHP = std::min<float>(hp, maxHP);

	if (mPlayerHpBar != nullptr)
	{
		mPlayerHpBar->SetHPRatio(GetSoulHpRatio());
	}
}

