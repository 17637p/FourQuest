#include "Soul.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Particle.h"
#include "DeadArmour.h"
#include "CameraMoving.h"
#include "ClientEvent.h"
#include "PlayerSoulVariable.h"
#include "Player.h"

#include "SoulVariable.h"

fq::client::Soul::Soul()
	:mController(nullptr)
	, mSelectArmours{}
	, mbIsDistanceInPlayer(false)
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

	// �ҿ� ���� ���� 
	SetSoulColor();
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

	checkOtherPlayer();

}

void fq::client::Soul::OnLateUpdate(float dt)
{
	mbIsDistanceInPlayer = false;
}

void fq::client::Soul::checkOtherPlayer()
{
	for (auto& object : GetScene()->GetObjectView())
	{
		if (object.HasComponent<Player>())
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

void fq::client::Soul::SetSoulType(fq::client::ESoulType type)
{
	mSoulType = type;

	SetSoulColor();
}

