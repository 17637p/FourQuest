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

	
	SetSoulColor();		// 소울 색깔 지정 
	SetSoulHP();		// 소울 HP
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
}

void fq::client::Soul::OnUpdate(float dt)
{
	selectArmour();
	checkOtherPlayer();
	updateSoulHP(dt);
	selectGoddessStatue(dt);
	checkReleaseGoddessStatue();
}

void fq::client::Soul::OnLateUpdate(float dt)
{
	mbIsDistanceInPlayer = false;
}

void fq::client::Soul::selectArmour()
{
	auto input = GetScene()->GetInputManager();

	if (!mSelectArmours.empty()
		&& input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold))
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

		PlayerInfo info{ mController->GetControllerID(), mSoulType };

		if (closestArmour->SummonLivingArmour(info))
		{
			DestorySoul();

			spdlog::trace("DestroySoul");
		}
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

	// 여신상에 빙의 중이라면
	if (mIsOverlayGoddessStatue)
	{
		return;
	}

	// 영혼 주변에 갑옷 플레이어가 있는지 체크 여부에 따라 HP 감소량 감소
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

	// 영혼 죽으면 오브젝트 삭제하고 소울 매니저한테 영혼 파괴되었다고 알림
	// 파티클을 없애고 리지드 바디 삭제 ( 다음 씬에 보내기 위해 해당 오브젝트를 삭제하면 안됨 ( Soul, CharacterController 데이터 저장 ))
	if (mHP <= 0.f)
	{
		for (auto& object : GetGameObject()->GetChildren())
		{
			if (object->HasComponent<fq::game_module::Particle>())
			{
				GetScene()->DestroyGameObject(object);
			}
		}

		// 이벤트로 영혼 파괴 되었음을 업데이트
		int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ id, EPlayerType::SoulDestoryed });

		// 콜라이더와 리지드 바디 삭제
		GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
		GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();

		// 카메라에 플레이어 해제 
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
			if (mSelectGoddessStatue != nullptr)
			{
				// 소울 위치를 여신상 위치로 해서 숨기기 
				auto soulT = GetComponent<game_module::Transform>();
				auto goddessStatuePos = mSelectGoddessStatue->GetComponent<game_module::Transform>()->GetWorldPosition();

				goddessStatuePos.y += 2.0f;
				soulT->SetWorldPosition(goddessStatuePos);

				bool isSuccessOverlay = mSelectGoddessStatue->SetOverlaySoul(true, this);

				if (isSuccessOverlay)
				{
					mIsOverlayGoddessStatue = true;
					// 빙의하면 못 움직이게 처리 
					GetComponent<game_module::CharacterController>()->SetCanMoveCharater(false);
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

	SetSoulHP();
}

void fq::client::Soul::checkReleaseGoddessStatue()
{
	if (mIsOverlayGoddessStatue)
	{
		auto input = GetScene()->GetInputManager();
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
		{
			// 영혼 최대 체력으로 
			SetSoulHP();
			ReleaseGoddessStatue();
		}
	}
}

