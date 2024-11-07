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

	if (cloneSoul == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneSoul = game_module::ObjectPool::GetInstance()->Assign<Soul>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSoul = *this;
	}

	return cloneSoul;
}

void fq::client::Soul::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();

	// Player���
	GetScene()->GetEventManager()->FireEvent<client::event::RegisterPlayer>(
		{ GetGameObject(), EPlayerType::Soul });

	// ī�޶� �÷��̾� ��� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

	// UI ���
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

	SetSoulColor();		// �ҿ� ���� ���� 
	setSoulHP();		// �ҿ� HP

	setName();

	mbIsOnSummon = false;
	mSummonArmourOrNull = nullptr;
	mDashElapsed = 0.f;
}

void fq::client::Soul::DestorySoul()
{
	GetScene()->DestroyGameObject(GetGameObject());

	// ī�޶� �÷��̾� ���� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Soul::OnTriggerEnter(const fq::game_module::Collision& collision)
{
	// deadArmour ó��
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();
	if (deadArmour != nullptr)
	{
		mSelectArmours.push_back(deadArmour);
	}

	// GoddessStatue ó��
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
	// deadArmour ó��
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();
	if (deadArmour != nullptr)
	{
		mSelectArmours.erase(std::remove(mSelectArmours.begin(), mSelectArmours.end(), deadArmour)
			, mSelectArmours.end()
		);
	}

	// GoddessStatue ó��
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
		processInput(dt); // ���� ���� ȣ�� �ʿ� (ĳ���� �̵�����)
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
	// ������ ���Ż�� ��ȣ�ۿ� �� ���� ���� �Ұ�����
	if (mSelectGoddessStatue != nullptr
		&& mSelectGoddessStatue->GetIsCorrupt())
	{
		return;
	}

	auto input = GetScene()->GetInputManager();

	if (!mSelectArmours.empty())
	{
		// ���� ����� ���� ����
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

		// ������ ���� �� �ִ��� ��Ÿ�� üũ ( UI ���� ���� )
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
			&& !mIsOverlayGoddessStatue) // ���Ż� ��ġ ��ó�� ���� ���� �� ���Ż����� �̵��ϰ� ���� �Ծ����� ����
		{
			PlayerInfo info{ mController->GetControllerID(), mSoulType };

			if (closestArmour->EnterSummonLivingArmour(info))
			{
				mbIsOnSummon = true;
				mSummonArmourOrNull = closestArmour;
				mController->SetCanMoveCharater(false);

				// ��ġ ����
				auto armourTransform = closestArmour->GetTransform();
				auto curTransform = GetTransform();
				curTransform->SetWorldMatrix(armourTransform->GetWorldMatrix());

				// �ӵ� ����
				auto rigidbody = GetComponent<fq::game_module::RigidBody>();
				if (rigidbody != nullptr)
				{
					rigidbody->SetLinearVelocity({ 0,0,0 });
				}
				
				// ���� ���
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_ArmorSpawn", false , fq::sound::EChannel::SE });

				mPlayerHpBar->SetVisible(false);
			}
		}
	}
}

void fq::client::Soul::checkOtherPlayer()
{
	// �Ÿ� ���� ������ ���� �ٸ� �÷��̾ Ž�� �� ���� �Ÿ� ���� ������ ��ȥ ����
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
	// ���Ż� ���� ���̰ų� ������ ���� �ִ� ���
	if (mIsOverlayGoddessStatue || mbIsInSafeZone)
	{
		return;
	}

	// ��ȥ �ֺ��� ���� �÷��̾ �ִ��� üũ ���ο� ���� HP ���ҷ� ����
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

	// ��ȥ ������ ������Ʈ �����ϰ� �ҿ� �Ŵ������� ��ȥ �ı��Ǿ��ٰ� �˸�
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
	// �浹�� ���Ż��� ���ų� ��ȭ�� ���Ż��̶�� ���� ����
	if (mSelectGoddessStatue == nullptr
		|| !mSelectGoddessStatue->GetIsCorrupt())
	{
		mCurHoldB = 0;
		return;
	}

	// �Է� ó��
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
				// �����ϸ� �� �����̰� ó�� 
				GetComponent<game_module::CharacterController>()->SetCanMoveCharater(false);

				if (mPlayerHpBar)
					mPlayerHpBar->SetVisible(false);

				// �ҿ� ��ġ�� ���Ż� ��ġ�� �ؼ� ����� 
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

	// ������ UI ���� ����
	mbIsVisibleBGaugeUI = true;
	mBGaugeFillAmount = mCurHoldB / mNeedHoldB * 360.f;
}

void fq::client::Soul::ReleaseGoddessStatue()
{
	mIsOverlayGoddessStatue = false;
	GetComponent<game_module::CharacterController>()->SetCanMoveCharater(true);

	// ���Ż� ��¦ ������ ��ȯ
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
			// ��ȥ �ִ� ü������ 
			ReleaseGoddessStatue();
		}

		// ������ UI ���� ����
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
	// ���� ���� ī���Ϳ� ���� ��ȥ �ִ� ü�� ����
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
			// �ӵ� ó��
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

