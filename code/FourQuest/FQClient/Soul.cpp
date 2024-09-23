#include "Soul.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Particle.h"
#include "../FQGameModule/GameObject.h"

#include "DeadArmour.h"
#include "CameraMoving.h"
#include "ClientEvent.h"
#include "PlayerSoulVariable.h"
#include "Player.h"
#include "SoulManager.h"
#include "HpBar.h"

#include "SoulVariable.h"

#include <algorithm>

fq::client::Soul::Soul()
	:mController(nullptr)
	, mSelectArmours{}
	, mbIsDistanceInPlayer(false)
	, mHP()
	, mDeathCount()
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

	
	SetSoulManager();	// 소울 매니저 등록
	SetSoulColor();		// 소울 색깔 지정 
	SetSoulDeath();		// 소울 죽음 카운트 계산
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
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();

	if (deadArmour == nullptr)
		return;

	mSelectArmours.push_back(deadArmour);
}

void fq::client::Soul::OnTriggerExit(const fq::game_module::Collision& collision)
{
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();

	if (deadArmour == nullptr)
		return;

	mSelectArmours.erase(std::remove(mSelectArmours.begin(), mSelectArmours.end(), deadArmour)
		, mSelectArmours.end()
	);
}

void fq::client::Soul::OnUpdate(float dt)
{
	selectArmour();
	checkOtherPlayer();
	updateSoulHP(dt);
}

void fq::client::Soul::OnLateUpdate(float dt)
{
	mbIsDistanceInPlayer = false;
}

void fq::client::Soul::selectArmour()
{
	auto input = GetScene()->GetInputManager();

	// 갑옷과 상호작용합니다.
	if (!mSelectArmours.empty()
		&& input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		// 가장 가까운 갑옷 쿼리
		auto soulPos = GetComponent<game_module::Transform>()->GetWorldPosition();
		float minDistance = 100000.f;
		DeadArmour* closestArmour = nullptr;

		for (auto& armour : mSelectArmours)
		{
			if (armour->GetGameObject()->IsDestroyed())
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

		closestArmour->SummonLivingArmour(info);
		DestorySoul();
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

void fq::client::Soul::SetSoulManager()
{
	bool bisSoulManager = false;

	// 씬에 소울 매니저 오브젝트가 있는지 확인
	for (auto& object : GetScene()->GetObjectView())
	{
		if (object.HasComponent<SoulManager>())
		{
			bisSoulManager = true;
			continue;
		}
	}

	// 씬에서 소울 오브젝트 컴포넌트를 가진 오브젝트 주소 가져오기
	if (bisSoulManager)
	{
		for (auto& object : GetScene()->GetComponentView<SoulManager>())
		{
			mSoulManagerObject = &object;
		}
	}
	else
	{
		std::shared_ptr<fq::game_module::GameObject> gameObject = std::make_shared<fq::game_module::GameObject>();
		gameObject->AddComponent<SoulManager>();

		GetScene()->AddGameObject(gameObject);
		mSoulManagerObject = gameObject.get();
	}
}

void fq::client::Soul::SetSoulHP()
{
	// 갑옷 죽음 카운터에 따라 영혼 최대 체력 세팅
	int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();

	int maxHP = SoulVariable::SoulMaxHp - SoulVariable::SoulHpDown * mDeathCount;
	int minHP = SoulVariable::SoulMinHp;

	mHP = std::max<int>(maxHP, minHP);
}

void fq::client::Soul::SetSoulDeath()
{
	// 갑옷 죽음 카운트 가져오기
	int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();

	if (mSoulManagerObject != nullptr)
	{
		auto soulManager = mSoulManagerObject->GetComponent<SoulManager>();
		mDeathCount = soulManager->GetPlayerDeathCount(id);
	}
	else
	{
		spdlog::error("Scene Have not SoulManager!!");
	}
}

void fq::client::Soul::updateSoulHP(float dt)
{
	mHP -= SoulVariable::SoulHpDecreas * dt;

	GetComponent<HpBar>()->DecreaseHp((SoulVariable::SoulHpDecreas * dt) / (float)SoulVariable::SoulMaxHp);

	// 영혼 죽으면 오브젝트 삭제하고 소울 매니저한테 죽었다고 알림
	if (mHP <= 0.f)
	{
		GetScene()->DestroyGameObject(GetGameObject());
		
		int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();
		auto soulManager = mSoulManagerObject->GetComponent<SoulManager>();

		soulManager->SetbIsPlayerSoulDeath(id, true);
	}
}

void fq::client::Soul::SetSoulType(fq::client::ESoulType type)
{
	mSoulType = type;

	SetSoulColor();
}

