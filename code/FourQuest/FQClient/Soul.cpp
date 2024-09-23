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

	
	SetSoulManager();	// �ҿ� �Ŵ��� ���
	SetSoulColor();		// �ҿ� ���� ���� 
	SetSoulDeath();		// �ҿ� ���� ī��Ʈ ���
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

	// ���ʰ� ��ȣ�ۿ��մϴ�.
	if (!mSelectArmours.empty()
		&& input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		// ���� ����� ���� ����
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

	// ���� �ҿ� �Ŵ��� ������Ʈ�� �ִ��� Ȯ��
	for (auto& object : GetScene()->GetObjectView())
	{
		if (object.HasComponent<SoulManager>())
		{
			bisSoulManager = true;
			continue;
		}
	}

	// ������ �ҿ� ������Ʈ ������Ʈ�� ���� ������Ʈ �ּ� ��������
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
	// ���� ���� ī���Ϳ� ���� ��ȥ �ִ� ü�� ����
	int id = GetComponent<fq::game_module::CharacterController>()->GetControllerID();

	int maxHP = SoulVariable::SoulMaxHp - SoulVariable::SoulHpDown * mDeathCount;
	int minHP = SoulVariable::SoulMinHp;

	mHP = std::max<int>(maxHP, minHP);
}

void fq::client::Soul::SetSoulDeath()
{
	// ���� ���� ī��Ʈ ��������
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

	// ��ȥ ������ ������Ʈ �����ϰ� �ҿ� �Ŵ������� �׾��ٰ� �˸�
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

