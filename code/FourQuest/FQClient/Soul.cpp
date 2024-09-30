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
#include "HpBar.h"

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

	// GaugeUI ���
	for (auto& child : GetGameObject()->GetChildren())
	{
		auto ui = child->GetComponent<BGaugeUI>();

		if (ui)
			mBGaugeUI = ui;
	}

	SetSoulColor();		// �ҿ� ���� ���� 
	SetSoulHP();		// �ҿ� HP
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
}

void fq::client::Soul::OnUpdate(float dt)
{
	selectGoddessStatue(dt);
	selectArmour();
	checkOtherPlayer();
	updateSoulHP(dt);
	checkReleaseGoddessStatue();
}

void fq::client::Soul::OnLateUpdate(float dt)
{
	mbIsDistanceInPlayer = false;
}

void fq::client::Soul::selectArmour()
{
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
			mBGaugeUI->SetVisible(false);
		}

		// ������ ���� �� �ִ��� ��Ÿ�� üũ ( UI ���� )
		if (mBGaugeUI)
		{
			mBGaugeUI->SetVisible(true);

			if (closestArmour->GetUnequippedPlayerId() == mController->GetControllerID())
			{
				mBGaugeUI->SetRatio((closestArmour->GetPlayerArmourCoolTime() / SoulVariable::ArmourCoolTime) * 360.f);
			}
			else
			{
				mBGaugeUI->SetRatio(360.f);
			}
		}

		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold)
			&& !mIsOverlayGoddessStatue) // ���Ż� ��ġ ��ó�� ���� ���� �� ���Ż����� �̵��ϰ� ���� �Ծ����� ����
		{
			PlayerInfo info{ mController->GetControllerID(), mSoulType };

			if (closestArmour->SummonLivingArmour(info))
			{
				DestorySoul();

				spdlog::trace("DestroySoul");
			}
		}
	}
	else if(mBGaugeUI)
	{
		mBGaugeUI->SetVisible(false);
	}
}

void fq::client::Soul::checkOtherPlayer()
{
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

void fq::client::Soul::SetSoulHP()
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

	int maxHP = SoulVariable::SoulMaxHp - SoulVariable::SoulHpDown * deathCount;
	int minHP = SoulVariable::SoulMinHp;

	mHP = std::max<int>(maxHP, minHP);

	if (GetGameObject()->HasComponent<HpBar>())
		GetComponent<HpBar>()->DecreaseHp(((SoulVariable::SoulMaxHp - mHP) / (float)SoulVariable::SoulMaxHp));
	else
		spdlog::error("ERROR : GameObject(this) have not \"HpBar\" Component!");
}

void fq::client::Soul::updateSoulHP(float dt)
{
	if (!GetGameObject()->HasComponent<HpBar>())
	{
		spdlog::error("ERROR : GameObject(this) have not \"HpBar\" Component!");
		return;
	}

	// ���Ż� ���� ���̶��
	if (mIsOverlayGoddessStatue)
	{
		return;
	}

	// ��ȥ �ֺ��� ���� �÷��̾ �ִ��� üũ ���ο� ���� HP ���ҷ� ����
	if (mbIsDistanceInPlayer)
	{
		float decreasPercentage = ((100.f - SoulVariable::SoulDecreasPercentage) / 100.f);
		float decreasDamage = SoulVariable::SoulHpDecreas * dt * decreasPercentage;

		mHP -= decreasDamage;
		GetComponent<HpBar>()->DecreaseHp((decreasDamage / (float)SoulVariable::SoulMaxHp));
	}
	else
	{
		mHP -= SoulVariable::SoulHpDecreas * dt;
		GetComponent<HpBar>()->DecreaseHp((SoulVariable::SoulHpDecreas * dt) / (float)SoulVariable::SoulMaxHp);
	}

	// ��ȥ ������ ������Ʈ �����ϰ� �ҿ� �Ŵ������� ��ȥ �ı��Ǿ��ٰ� �˸�
	// ��ƼŬ�� ���ְ� ������ �ٵ� ���� ( ���� ���� ������ ���� �ش� ������Ʈ�� �����ϸ� �ȵ� ( Soul, CharacterController ������ ���� ))
	if (mHP <= 0.f)
	{
		for (auto& object : GetGameObject()->GetChildren())
		{
			if (object->HasComponent<fq::game_module::Particle>())
			{
				GetScene()->DestroyGameObject(object);
			}
		}

		// �̺�Ʈ�� ��ȥ �ı� �Ǿ����� ������Ʈ
		int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ id, EPlayerType::SoulDestoryed });

		// �ݶ��̴��� ������ �ٵ� ����
		GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
		GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();

		// ī�޶� �÷��̾� ���� 
		GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
			{
				camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
			});
	}
}

void fq::client::Soul::SetSoulType(fq::client::ESoulType type)
{
	mSoulType = type;

	SetSoulColor();
}

void fq::client::Soul::selectGoddessStatue(float dt)
{
	auto input = GetScene()->GetInputManager();
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold))
	{
		mCurHoldB += dt;
		if (mCurHoldB > mNeedHoldB)
		{
			mCurHoldB = 0;
			if (mSelectGoddessStatue != nullptr && mSelectGoddessStatue->GetIsCorrupt())
			{
				bool isSuccessOverlay = mSelectGoddessStatue->SetOverlaySoul(true, this);

				if (isSuccessOverlay)
				{
					mIsOverlayGoddessStatue = true;
					// �����ϸ� �� �����̰� ó�� 
					GetComponent<game_module::CharacterController>()->SetCanMoveCharater(false);

					// �ҿ� ��ġ�� ���Ż� ��ġ�� �ؼ� ����� 
					auto soulT = GetComponent<game_module::Transform>();
					auto goddessStatuePos = mSelectGoddessStatue->GetComponent<game_module::Transform>()->GetWorldPosition();

					goddessStatuePos.y += 2.0f;
					soulT->SetWorldPosition(goddessStatuePos);
				}
			}
		}
	}
	else
	{
		mCurHoldB = 0;
	}
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

	SetSoulHP();
}

void fq::client::Soul::checkReleaseGoddessStatue()
{
	if (mIsOverlayGoddessStatue)
	{
		auto input = GetScene()->GetInputManager();
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
		{
			// ��ȥ �ִ� ü������ 
			SetSoulHP();
			ReleaseGoddessStatue();
		}
	}
}

float fq::client::Soul::GetSoulHpRatio() const
{
	return mHP / static_cast<float>(SoulVariable::SoulMaxHp);
}

